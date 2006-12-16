/*	$OpenBSD: src/usr.sbin/hostated/Attic/hce.c,v 1.2 2006/12/16 12:42:14 reyk Exp $	*/

/*
 * Copyright (c) 2006 Pierre-Yves Ritschard <pyr@spootnik.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/queue.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <errno.h>
#include <event.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <pwd.h>

#include "hostated.h"

void	hce_sig_handler(int sig, short, void *);
void	hce_shutdown(void);
void	hce_dispatch_imsg(int, short, void *);
void	hce_dispatch_parent(int, short, void *);
void	hce_launch_checks(int, short, void *);

static struct hostated		*env = NULL;
struct imsgbuf		*ibuf_pfe;
struct imsgbuf		*ibuf_main;

void
hce_sig_handler(int sig, short event, void *arg)
{
	switch (sig) {
	case SIGINT:
	case SIGTERM:
		hce_shutdown();
	default:
		fatalx("hce_sig_handler: unexpected signal");
	}
}

pid_t
hce(struct hostated *x_env, int pipe_parent2pfe[2], int pipe_parent2hce[2],
	int pipe_pfe2hce[2])
{
	pid_t		 pid;
	struct passwd	*pw;
	struct timeval	 tv;
	struct event	 ev_sigint;
	struct event	 ev_sigterm;

	switch (pid = fork()) {
	case -1:
		fatal("hce: cannot fork");
	case 0:
		break;
	default:
		return (pid);
	}

	env = x_env;

	/* this is needed for icmp tests */
	if ((env->icmp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
		err(1, "socket");
	if ((env->icmp6_sock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0)
		err(1, "socket");

	if ((pw = getpwnam(HOSTATED_USER)) == NULL)
		fatal("hce: getpwnam");

	if (chroot(pw->pw_dir) == -1)
		fatal("hce: chroot");
	if (chdir("/") == -1)
		fatal("hce: chdir(\"/\")");

	setproctitle("host check engine");
	hostated_process = PROC_HCE;

	if (setgroups(1, &pw->pw_gid) ||
	    setresgid(pw->pw_gid, pw->pw_gid, pw->pw_gid) ||
	    setresuid(pw->pw_uid, pw->pw_uid, pw->pw_uid))
		fatal("hce: can't drop privileges");

	event_init();

	signal_set(&ev_sigint, SIGINT, hce_sig_handler, NULL);
	signal_set(&ev_sigterm, SIGTERM, hce_sig_handler, NULL);
	signal_add(&ev_sigint, NULL);
	signal_add(&ev_sigterm, NULL);

	/* setup pipes */
	close(pipe_pfe2hce[1]);
	close(pipe_parent2hce[0]);
	close(pipe_parent2pfe[0]);
	close(pipe_parent2pfe[1]);

	if ((ibuf_pfe = calloc(1, sizeof(struct imsgbuf))) == NULL ||
	    (ibuf_main = calloc(1, sizeof(struct imsgbuf))) == NULL)
		fatal("hce");
	imsg_init(ibuf_pfe, pipe_pfe2hce[0], hce_dispatch_imsg);
	imsg_init(ibuf_main, pipe_parent2hce[1], hce_dispatch_parent);

	ibuf_pfe->events = EV_READ;
	event_set(&ibuf_pfe->ev, ibuf_pfe->fd, ibuf_pfe->events,
	    ibuf_pfe->handler, ibuf_pfe);
	event_add(&ibuf_pfe->ev, NULL);

	ibuf_main->events = EV_READ;
	event_set(&ibuf_main->ev, ibuf_main->fd, ibuf_main->events,
	    ibuf_main->handler, ibuf_main);
	event_add(&ibuf_main->ev, NULL);

	evtimer_set(&env->ev, hce_launch_checks, NULL);
	tv.tv_sec = env->interval;
	tv.tv_usec = 0;
	evtimer_add(&env->ev, &tv);

	hce_launch_checks(0, 0, NULL);
	event_dispatch();

	hce_shutdown();

	return (0);
}

void
hce_launch_checks(int fd, short event, void *arg)
{
	int			 previous_up;
	struct host		*host;
	struct table		*table;
	struct ctl_status	 st;
	struct timeval		 tv;

	tv.tv_sec = env->interval;
	tv.tv_usec = 0;
	evtimer_add(&env->ev, &tv);
	bzero(&st, sizeof(st));
	TAILQ_FOREACH(table, &env->tables, entry) {
		if (table->flags & F_DISABLE)
			continue;
		TAILQ_FOREACH(host, &table->hosts, entry) {
			if (host->flags & F_DISABLE)
				continue;
			previous_up = host->up;
			switch (table->check) {
			case CHECK_ICMP:
				host->up = check_icmp(host, env->icmp_sock,
				    env->icmp6_sock, table->timeout);
				break;
			case CHECK_TCP:
				host->up = check_tcp(host, table);
				break;
			case CHECK_HTTP_CODE:
				host->up = check_http_code(host, table);
				break;
			case CHECK_HTTP_DIGEST:
				host->up = check_http_digest(host, table);
				break;
			default:
				fatalx("hce_launch_checks: unknown check type");
				break;
			}
			if (host->up != previous_up) {
				st.id = host->id;
				st.up = host->up;
				imsg_compose(ibuf_pfe, IMSG_HOST_STATUS, 0, 0,
				    &st, sizeof(st));
			}
		}
	}
	/* tell pfe we're finished */
	imsg_compose(ibuf_pfe, IMSG_SYNC, 0, 0, NULL, 0);
}

void
hce_shutdown(void)
{
	log_info("host check engine exiting");
	_exit(0);
}

void
hce_dispatch_imsg(int fd, short event, void *ptr)
{
	struct imsgbuf		*ibuf;
	struct imsg		 imsg;
	ssize_t			 n;
	objid_t			 id;
	struct host		*host;
	struct table		*table;

	ibuf = ptr;
	switch (event) {
	case EV_READ:
		if ((n = imsg_read(ibuf)) == -1)
			fatal("hce_dispatch_imsg: imsg_read_error");
		if (n == 0)
			fatalx("hce_dispatch_imsg: pipe closed");
		break;
	case EV_WRITE:
		if (msgbuf_write(&ibuf->w) == -1)
			fatal("hce_dispatch_imsg: msgbuf_write");
		imsg_event_add(ibuf);
		return;
	default:
		fatalx("hce_dispatch_imsg: unknown event");
	}

	for (;;) {
		if ((n = imsg_get(ibuf, &imsg)) == -1)
			fatal("hce_dispatch_imsg: imsg_read error");
		if (n == 0)
			break;

		switch (imsg.hdr.type) {
		case IMSG_HOST_DISABLE:
			memcpy(&id, imsg.data, sizeof(id));
			if ((host = host_find(env, id)) == NULL)
				fatalx("hce_dispatch_imsg: desynchronized");
			host->flags |= F_DISABLE;
			host->up = HOST_UNKNOWN;
			break;
		case IMSG_HOST_ENABLE:
			memcpy(&id, imsg.data, sizeof(id));
			if ((host = host_find(env, id)) == NULL)
				fatalx("hce_dispatch_imsg: desynchronized");
			host->flags &= ~(F_DISABLE);
			host->up = HOST_UNKNOWN;
			break;
		case IMSG_TABLE_DISABLE:
			memcpy(&id, imsg.data, sizeof(id));
			if ((table = table_find(env, id)) == NULL)
				fatalx("hce_dispatch_imsg: desynchronized");
			table->flags |= F_DISABLE;
			TAILQ_FOREACH(host, &table->hosts, entry)
				host->up = HOST_UNKNOWN;
			break;
		case IMSG_TABLE_ENABLE:
			memcpy(&id, imsg.data, sizeof(id));
			if ((table = table_find(env, id)) == NULL)
				fatalx("hce_dispatch_imsg: desynchronized");
			table->flags &= ~(F_DISABLE);
			TAILQ_FOREACH(host, &table->hosts, entry)
				host->up = HOST_UNKNOWN;
			break;
		default:
			log_debug("hce_dispatch_msg: unexpected imsg %d",
				imsg.hdr.type);
			break;
		}
		imsg_free(&imsg);
	}
	imsg_event_add(ibuf);
}

void
hce_dispatch_parent(int fd, short event, void * ptr)
{
	struct imsgbuf	*ibuf;
	struct imsg	 imsg;
	ssize_t		 n;

	ibuf = ptr;
	switch (event) {
	case EV_READ:
		if ((n = imsg_read(ibuf)) == -1)
			fatal("hce_dispatch_parent: imsg_read error");
		if (n == 0)     /* connection closed */
			fatalx("hce_dispatch_parent: pipe closed");
		break;
	case EV_WRITE:
		if (msgbuf_write(&ibuf->w) == -1)
			fatal("hce_dispatch_parent: msgbuf_write");
		imsg_event_add(ibuf);
		return;
	default:
		fatalx("hce_dispatch_parent: unknown event");
	}

	for (;;) {
		if ((n = imsg_get(ibuf, &imsg)) == -1)
			fatal("hce_dispatch_parent: imsg_read error");
		if (n == 0)
			break;

		switch (imsg.hdr.type) {
		default:
			log_debug("hce_dispatch_parent: unexpected imsg %d",
			    imsg.hdr.type);
			break;
		}
		imsg_free(&imsg);
	}
}
