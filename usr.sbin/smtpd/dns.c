/*	$OpenBSD: src/usr.sbin/smtpd/dns.c,v 1.65 2013/04/30 12:07:21 eric Exp $	*/

/*
 * Copyright (c) 2008 Gilles Chehade <gilles@poolp.org>
 * Copyright (c) 2009 Jacek Masiulaniec <jacekm@dobremiasto.net>
 * Copyright (c) 2011-2012 Eric Faurot <eric@faurot.net>
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/tree.h>
#include <sys/queue.h>
#include <sys/uio.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>

#include <event.h>
#include <imsg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asr.h"
#include "asr_private.h"
#include "smtpd.h"
#include "log.h"

struct dns_lookup {
	struct dns_session	*session;
	int			 preference;
};

struct dns_session {
	struct mproc		*p;
	uint64_t		 reqid;
	int			 type;
	char			 name[MAXHOSTNAMELEN];
	size_t			 mxfound;
	int			 error;
	int			 refcount;
};

struct async_event;
struct async_event * async_run_event(struct async *,
	void (*)(int, struct async_res *, void *), void *);

static void dns_lookup_host(struct dns_session *, const char *, int);
static void dns_dispatch_host(int, struct async_res *, void *);
static void dns_dispatch_ptr(int, struct async_res *, void *);
static void dns_dispatch_mx(int, struct async_res *, void *);
static void dns_dispatch_mx_preference(int, struct async_res *, void *);

#define print_dname(a,b,c) asr_strdname(a, b, c)

void
dns_query_host(uint64_t id, const char *host)
{
	m_create(p_lka,  IMSG_DNS_HOST, 0, 0, -1, 128);
	m_add_id(p_lka, id);
	m_add_string(p_lka, host);
	m_close(p_lka);
}

void
dns_query_ptr(uint64_t id, const struct sockaddr *sa)
{
	m_create(p_lka,  IMSG_DNS_PTR, 0, 0, -1, 128);
	m_add_id(p_lka, id);
	m_add_sockaddr(p_lka, sa);
	m_close(p_lka);
}

void
dns_query_mx(uint64_t id, const char *domain)
{
	m_create(p_lka,  IMSG_DNS_MX, 0, 0, -1, 128);
	m_add_id(p_lka, id);
	m_add_string(p_lka, domain);
	m_close(p_lka);
}

void
dns_query_mx_preference(uint64_t id, const char *domain, const char *mx)
{
	m_create(p_lka,  IMSG_DNS_MX_PREFERENCE, 0, 0, -1, 128);
	m_add_id(p_lka, id);
	m_add_string(p_lka, domain);
	m_add_string(p_lka, mx);
	m_close(p_lka);
}

void
dns_imsg(struct mproc *p, struct imsg *imsg)
{
	struct sockaddr_storage	 ss;
	struct dns_session	*s;
	struct sockaddr		*sa;
	struct async		*as;
	struct msg		 m;
	const char		*domain, *mx, *host;

	s = xcalloc(1, sizeof *s, "dns_imsg");
	s->type = imsg->hdr.type;
	s->p = p;

	m_msg(&m, imsg);
	m_get_id(&m, &s->reqid);

	switch (s->type) {

	case IMSG_DNS_HOST:
		m_get_string(&m, &host);
		m_end(&m);
		dns_lookup_host(s, host, -1);
		return;

	case IMSG_DNS_PTR:
		sa = (struct sockaddr *)&ss;
		m_get_sockaddr(&m, sa);
		m_end(&m);
		as = getnameinfo_async(sa, sa->sa_len, s->name, sizeof(s->name),
		    NULL, 0, 0, NULL);
		async_run_event(as, dns_dispatch_ptr, s);
		return;

	case IMSG_DNS_MX:
		m_get_string(&m, &domain);
		m_end(&m);
		strlcpy(s->name, domain, sizeof(s->name));
		as = res_query_async(s->name, C_IN, T_MX, NULL);
		async_run_event(as, dns_dispatch_mx, s);
		return;

	case IMSG_DNS_MX_PREFERENCE:
		m_get_string(&m, &domain);
		m_get_string(&m, &mx);
		m_end(&m);
		strlcpy(s->name, mx, sizeof(s->name));
		as = res_query_async(domain, C_IN, T_MX, NULL);
		async_run_event(as, dns_dispatch_mx_preference, s);
		return;

	default:
		log_warnx("warn: bad dns request %i", s->type);
		fatal(NULL);
	}
}

static void
dns_dispatch_host(int ev, struct async_res *ar, void *arg)
{
	struct dns_session	*s;
	struct dns_lookup	*lookup = arg;
	struct addrinfo		*ai;

	s = lookup->session;

	for (ai = ar->ar_addrinfo; ai; ai = ai->ai_next) {
		s->mxfound++;
		m_create(s->p, IMSG_DNS_HOST, 0, 0, -1, 128);
		m_add_id(s->p, s->reqid);
		m_add_sockaddr(s->p, ai->ai_addr);
		m_add_int(s->p, lookup->preference);
		m_close(s->p);
	}
	free(lookup);
	if (ar->ar_addrinfo)
		freeaddrinfo(ar->ar_addrinfo);

	if (ar->ar_gai_errno)
		s->error = ar->ar_gai_errno;

	if (--s->refcount)
		return;

	m_create(s->p, IMSG_DNS_HOST_END, 0, 0, -1, 24);
	m_add_id(s->p, s->reqid);
	m_add_int(s->p, s->mxfound ? DNS_OK : DNS_ENOTFOUND);
	m_close(s->p);
	free(s);
}

static void
dns_dispatch_ptr(int ev, struct async_res *ar, void *arg)
{
	struct dns_session	*s = arg;

	/* The error code could be more precise, but we don't currently care */
	m_create(s->p,  IMSG_DNS_PTR, 0, 0, -1, 512);
	m_add_id(s->p, s->reqid);
	m_add_int(s->p, ar->ar_gai_errno ? DNS_ENOTFOUND : DNS_OK);
	if (ar->ar_gai_errno == 0)
		m_add_string(s->p, s->name);
	m_close(s->p);
	free(s);
}

static void
dns_dispatch_mx(int ev, struct async_res *ar, void *arg)
{
	struct dns_session	*s = arg;
	struct unpack		 pack;
	struct header		 h;
	struct query		 q;
	struct rr		 rr;
	char			 buf[512];
	size_t			 found;

	if (ar->ar_h_errno && ar->ar_h_errno != NO_DATA) {

		m_create(s->p,  IMSG_DNS_HOST_END, 0, 0, -1, 24);
		m_add_id(s->p, s->reqid);
		if (ar->ar_rcode == NXDOMAIN)
			m_add_int(s->p, DNS_ENONAME);
		else if (ar->ar_h_errno == NO_RECOVERY)
			m_add_int(s->p, DNS_EINVAL);
		else
			m_add_int(s->p, DNS_RETRY);
		m_close(s->p);
		free(s);
		free(ar->ar_data);
		return;
	}

	unpack_init(&pack, ar->ar_data, ar->ar_datalen);
	unpack_header(&pack, &h);
	unpack_query(&pack, &q);

	found = 0;
	for (; h.ancount; h.ancount--) {
		unpack_rr(&pack, &rr);
		if (rr.rr_type != T_MX)
			continue;
		print_dname(rr.rr.mx.exchange, buf, sizeof(buf));
		buf[strlen(buf) - 1] = '\0';
		dns_lookup_host(s, buf, rr.rr.mx.preference);
		found++;
	}
	free(ar->ar_data);

	/* fallback to host if no MX is found. */
	if (found == 0)
		dns_lookup_host(s, s->name, 0);
}

static void
dns_dispatch_mx_preference(int ev, struct async_res *ar, void *arg)
{
	struct dns_session	*s = arg;
	struct unpack		 pack;
	struct header		 h;
	struct query		 q;
	struct rr		 rr;
	char			 buf[512];
	int			 error;

	if (ar->ar_h_errno) {
		if (ar->ar_rcode == NXDOMAIN)
			error = DNS_ENONAME;
		else if (ar->ar_h_errno == NO_RECOVERY
		    || ar->ar_h_errno == NO_DATA)
			error = DNS_EINVAL;
		else
			error = DNS_RETRY;
	}
	else {
		error = DNS_ENOTFOUND;
		unpack_init(&pack, ar->ar_data, ar->ar_datalen);
		unpack_header(&pack, &h);
		unpack_query(&pack, &q);
		for (; h.ancount; h.ancount--) {
			unpack_rr(&pack, &rr);
			if (rr.rr_type != T_MX)
				continue;
			print_dname(rr.rr.mx.exchange, buf, sizeof(buf));
			buf[strlen(buf) - 1] = '\0';
			if (!strcasecmp(s->name, buf)) {
				error = DNS_OK;
				break;
			}
		}
	}

	free(ar->ar_data);

	m_create(s->p, IMSG_DNS_MX_PREFERENCE, 0, 0, -1, 36);
	m_add_id(s->p, s->reqid);
	m_add_int(s->p, error);
	if (error == DNS_OK)
		m_add_int(s->p, rr.rr.mx.preference);
	m_close(s->p);
	free(s);
}

static void
dns_lookup_host(struct dns_session *s, const char *host, int preference)
{
	struct dns_lookup	*lookup;
	struct addrinfo		 hints;
	struct async		*as;

	lookup = xcalloc(1, sizeof *lookup, "dns_lookup_host");
	lookup->preference = preference;
	lookup->session = s;
	s->refcount++;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	as = getaddrinfo_async(host, NULL, &hints, NULL);
	async_run_event(as, dns_dispatch_host, lookup);
}

/* Generic libevent glue for asr */

struct async_event {
	struct async	*async;
	struct event	 ev;
	void		(*callback)(int, struct async_res *, void *);
	void		*arg;
};

static void async_event_dispatch(int, short, void *);

struct async_event *
async_run_event(struct async * async,
    void (*cb)(int, struct async_res *, void *), void *arg)
{
	struct async_event	*aev;
	struct timeval		 tv;

	aev = calloc(1, sizeof *aev);
	if (aev == NULL)
		return (NULL);
	aev->async = async;
	aev->callback = cb;
	aev->arg = arg;
	tv.tv_sec = 0;
	tv.tv_usec = 1;
	evtimer_set(&aev->ev, async_event_dispatch, aev);
	evtimer_add(&aev->ev, &tv);
	return (aev);
}

static void
async_event_dispatch(int fd, short ev, void *arg)
{
	struct async_event	*aev = arg;
	struct async_res	 ar;
	int			 r;
	struct timeval		 tv;

	while ((r = async_run(aev->async, &ar)) == ASYNC_YIELD)
		aev->callback(r, &ar, aev->arg);

	event_del(&aev->ev);
	if (r == ASYNC_COND) {
		event_set(&aev->ev, ar.ar_fd,
			  ar.ar_cond == ASYNC_READ ? EV_READ : EV_WRITE,
			  async_event_dispatch, aev);
		tv.tv_sec = ar.ar_timeout / 1000;
		tv.tv_usec = (ar.ar_timeout % 1000) * 1000;
		event_add(&aev->ev, &tv);
	} else { /* ASYNC_DONE */
		aev->callback(r, &ar, aev->arg);
		free(aev);
	}
}
