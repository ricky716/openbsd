/*	$OpenBSD: src/sbin/ifconfig/ifconfig.c,v 1.17 1998/07/07 07:26:15 deraadt Exp $	*/
/*      $NetBSD: ifconfig.c,v 1.40 1997/10/01 02:19:43 enami Exp $      */

/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static char copyright[] =
"@(#) Copyright (c) 1983, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)ifconfig.c	8.2 (Berkeley) 2/16/94";
#else
static char rcsid[] = "$OpenBSD: src/sbin/ifconfig/ifconfig.c,v 1.17 1998/07/07 07:26:15 deraadt Exp $";
#endif
#endif /* not lint */

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netatalk/at.h>

#define	NSIP
#include <netns/ns.h>
#include <netns/ns_if.h>

#define	IPXIP
#include <netipx/ipx.h>
#include <netipx/ipx_if.h>

#include <netdb.h>

#define EON
#include <netiso/iso.h>
#include <netiso/iso_var.h>
#include <sys/protosw.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct	ifreq		ifr, ridreq;
struct	ifaliasreq	addreq;
struct	iso_ifreq	iso_ridreq;
struct	iso_aliasreq	iso_addreq;
struct	sockaddr_in	netmask;
struct  netrange	at_nr;		/* AppleTalk net range */

int	ipx_type = ETHERTYPE_II;
char	name[30];
int	flags, metric, mtu, setaddr, setipdst, doalias;
int	clearaddr, s;
int	newaddr = 1;
int	nsellength = 1;
int	af = AF_INET;
int     dflag, mflag, lflag, uflag;
int     reset_if_flags;

void 	notealias __P((char *, int));
void 	notrailers __P((char *, int));
void 	setifaddr __P((char *, int));
void 	setifdstaddr __P((char *, int));
void 	setifflags __P((char *, int));
void 	setifbroadaddr __P((char *));
void 	setifipdst __P((char *));
void 	setifmetric __P((char *));
void 	setifnetmask __P((char *));
void 	setnsellength __P((char *));
void 	setsnpaoffset __P((char *));
void	setipxframetype __P((char *, int));
void    setatrange __P((char *, int));
void    setatphase __P((char *, int));  
void    checkatrange __P ((struct sockaddr_at *));

#define	NEXTARG		0xffffff

struct	cmd {
	char	*c_name;
	int	c_parameter;		/* NEXTARG means next argv */
	void	(*c_func)();
} cmds[] = {
	{ "up",		IFF_UP,		setifflags } ,
	{ "down",	-IFF_UP,	setifflags },
	{ "trailers",	-1,		notrailers },
	{ "-trailers",	1,		notrailers },
	{ "arp",	-IFF_NOARP,	setifflags },
	{ "-arp",	IFF_NOARP,	setifflags },
	{ "debug",	IFF_DEBUG,	setifflags },
	{ "-debug",	-IFF_DEBUG,	setifflags },
	{ "alias",	IFF_UP,		notealias },
	{ "-alias",	-IFF_UP,	notealias },
	{ "delete",	-IFF_UP,	notealias },
#ifdef notdef
#define	EN_SWABIPS	0x1000
	{ "swabips",	EN_SWABIPS,	setifflags },
	{ "-swabips",	-EN_SWABIPS,	setifflags },
#endif
	{ "netmask",	NEXTARG,	setifnetmask },
	{ "metric",	NEXTARG,	setifmetric },
	{ "broadcast",	NEXTARG,	setifbroadaddr },
	{ "ipdst",	NEXTARG,	setifipdst },
#ifndef INET_ONLY
	{ "range",	NEXTARG,	setatrange },
	{ "phase",	NEXTARG,	setatphase },
	{ "snpaoffset",	NEXTARG,	setsnpaoffset },
	{ "nsellength",	NEXTARG,	setnsellength },
	{ "802.2",	ETHERTYPE_8022,	setipxframetype },
	{ "802.2tr",	ETHERTYPE_8022TR, setipxframetype },
	{ "802.3",	ETHERTYPE_8023,	setipxframetype },
	{ "snap",	ETHERTYPE_SNAP,	setipxframetype },
	{ "EtherII",	ETHERTYPE_II,	setipxframetype },
#endif	/* INET_ONLY */
	{ "link0",	IFF_LINK0,	setifflags } ,
	{ "-link0",	-IFF_LINK0,	setifflags } ,
	{ "link1",	IFF_LINK1,	setifflags } ,
	{ "-link1",	-IFF_LINK1,	setifflags } ,
	{ "link2",	IFF_LINK2,	setifflags } ,
	{ "-link2",	-IFF_LINK2,	setifflags } ,
	{ 0,		0,		setifaddr },
	{ 0,		0,		setifdstaddr },
};

void 	adjust_nsellength();
int	getinfo __P((struct ifreq *));
void	getsock __P((int));
void	printif __P((struct ifreq *, int));
void 	printb __P((char *, unsigned short, char *));
void 	status __P((int));
void 	usage();

/*
 * XNS support liberally adapted from code written at the University of
 * Maryland principally by James O'Toole and Chris Torek.
 */
void	in_status __P((int));
void 	in_getaddr __P((char *, int));
void    at_status __P((int));
void    at_getaddr __P((char *, int));
void 	xns_status __P((int));
void 	xns_getaddr __P((char *, int));
void 	ipx_status __P((int));
void 	ipx_getaddr __P((char *, int));
void 	iso_status __P((int));
void 	iso_getaddr __P((char *, int));

/* Known address families */
struct afswtch {
	char *af_name;
	short af_af;
	void (*af_status)();
	void (*af_getaddr)();
	u_long af_difaddr;
	u_long af_aifaddr;
	caddr_t af_ridreq;
	caddr_t af_addreq;
} afs[] = {
#define C(x) ((caddr_t) &x)
	{ "inet", AF_INET, in_status, in_getaddr,
	     SIOCDIFADDR, SIOCAIFADDR, C(ridreq), C(addreq) },
#ifndef INET_ONLY	/* small version, for boot media */
	{ "atalk", AF_APPLETALK, at_status, at_getaddr,
	    SIOCDIFADDR, SIOCAIFADDR, C(addreq), C(addreq) },
	{ "ns", AF_NS, xns_status, xns_getaddr,
	     SIOCDIFADDR, SIOCAIFADDR, C(ridreq), C(addreq) },
	{ "ipx", AF_IPX, ipx_status, ipx_getaddr,
	     SIOCDIFADDR, SIOCAIFADDR, C(ridreq), C(addreq) },
	{ "iso", AF_ISO, iso_status, iso_getaddr,
	     SIOCDIFADDR_ISO, SIOCAIFADDR_ISO, C(iso_ridreq), C(iso_addreq) },
#endif	/* INET_ONLY */
	{ 0,	0,	    0,		0 }
};

struct afswtch *afp;	/*the address family being set or asked about*/

int
main(argc, argv)
	int argc;
	char *argv[];
{
	register struct afswtch *rafp;
	int aflag = 0;
	int ifaliases = 0;

	if (argc < 2) 
		usage();
	argc--, argv++;
	if (!strcmp(*argv, "-a"))
		aflag = 1;
	else if (!strcmp(*argv, "-A")) {
		aflag = 1;
		ifaliases = 1;
	} else
		strncpy(name, *argv, sizeof(name));
	argc--, argv++;
	if (argc > 0) {
		for (afp = rafp = afs; rafp->af_name; rafp++)
			if (strcmp(rafp->af_name, *argv) == 0) {
				afp = rafp; argc--; argv++;
				break;
			}
		rafp = afp;
		af = ifr.ifr_addr.sa_family = rafp->af_af;
	}
	if (aflag) {
		if (argc > 0)
			usage();
		printif(NULL, ifaliases);
		exit(0);
	}
	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	if (argc == 0) {
		printif(&ifr, 1);
		exit(0);
	}

	if (getinfo(&ifr) < 0)
		exit(1);
	while (argc > 0) {
		register struct cmd *p;

		for (p = cmds; p->c_name; p++)
			if (strcmp(*argv, p->c_name) == 0)
				break;
		if (p->c_name == 0 && setaddr)
			p++;	/* got src, do dst */
		if (p->c_func) {
			if (p->c_parameter == NEXTARG) {
				if (argv[1] == NULL)
					errx(1, "'%s' requires argument",
					    p->c_name);
				(*p->c_func)(argv[1]);
				argc--, argv++;
			} else
				(*p->c_func)(*argv, p->c_parameter);
		}
		argc--, argv++;
	}

#ifndef INET_ONLY

	switch (af) {
	case AF_ISO:
		adjust_nsellength();
		break;
	case AF_NS:
		if (setipdst) {
			struct nsip_req rq;
			int size = sizeof(rq);

			rq.rq_ns = addreq.ifra_addr;
			rq.rq_ip = addreq.ifra_dstaddr;

			if (setsockopt(s, 0, SO_NSIP_ROUTE, &rq, size) < 0)
				warn("encapsulation routing");
		}
		break;
	case AF_IPX:
		if (setipdst) {
			struct ipxip_req rq;
			int size = sizeof(rq);

			rq.rq_ipx = addreq.ifra_addr;
			rq.rq_ip = addreq.ifra_dstaddr;

			if (setsockopt(s, 0, SO_IPXIP_ROUTE, &rq, size) < 0)
				warn("encapsulation routing");
		}
		break;
	case AF_APPLETALK:
		checkatrange((struct sockaddr_at *) &addreq.ifra_addr);
		break;
	}
#endif	/* INET_ONLY */

	if (clearaddr) {
		int ret;
		strncpy(rafp->af_ridreq, name, sizeof ifr.ifr_name);
		if ((ret = ioctl(s, rafp->af_difaddr, rafp->af_ridreq)) < 0) {
			if (errno == EADDRNOTAVAIL && (doalias >= 0)) {
				/* means no previous address for interface */
			} else
				warn("SIOCDIFADDR");
		}
	}
	if (newaddr) {
		strncpy(rafp->af_addreq, name, sizeof ifr.ifr_name);
		if (ioctl(s, rafp->af_aifaddr, rafp->af_addreq) < 0)
			warn("SIOCAIFADDR");
	}
	exit(0);
}

void
getsock(naf)
	int naf;
{
	static int oaf = -1;

	if (oaf == naf)
		return;
	if (oaf != -1)
		close(s);
	s = socket(naf, SOCK_DGRAM, 0);
	if (s < 0)
		oaf = -1;
	else
		oaf = naf;
}

int
getinfo(ifr)
	struct ifreq *ifr;
{

	getsock(af);
	if (s < 0)
		err(1, "socket");
	if (ioctl(s, SIOCGIFFLAGS, (caddr_t)ifr) < 0) {
		warn("SIOCGIFFLAGS");
		return (-1);
	}
	flags = ifr->ifr_flags;
	if (ioctl(s, SIOCGIFMETRIC, (caddr_t)ifr) < 0) {
		warn("SIOCGIFMETRIC");
		metric = 0;
	} else
		metric = ifr->ifr_metric;
	return (0);
}

void
printif(ifrm, ifaliases)
	struct ifreq *ifrm;
{
	char *inbuf = NULL;
	struct ifconf ifc;
	struct ifreq ifreq, *ifrp;
	int i, len = 8192;
	int count = 0, noinet = 1;

	getsock(af);
	if (s < 0)
		err(1, "socket");
	while (1) {
		ifc.ifc_len = len;
		ifc.ifc_buf = inbuf = realloc(inbuf, len);
		if (inbuf == NULL)
			err(1, "malloc");
		if (ioctl(s, SIOCGIFCONF, &ifc) < 0)
			err(1, "SIOCGIFCONF");
		if (ifc.ifc_len + sizeof(ifreq) < len)
			break;
		len *= 2;
	}
	ifrp = ifc.ifc_req;
	ifreq.ifr_name[0] = '\0';
	for (i = 0; i < ifc.ifc_len; ) {
		ifrp = (struct ifreq *)((caddr_t)ifc.ifc_req + i);
		i += sizeof(ifrp->ifr_name) +
			(ifrp->ifr_addr.sa_len > sizeof(struct sockaddr)
				? ifrp->ifr_addr.sa_len
				: sizeof(struct sockaddr));

		if (ifrm && strncmp(ifrm->ifr_name, ifrp->ifr_name,
		    sizeof(ifrp->ifr_name)))
			continue;
		strncpy(name, ifrp->ifr_name, sizeof(ifrp->ifr_name));
		if (ifrp->ifr_addr.sa_family == AF_LINK) {
			ifreq = ifr = *ifrp;
			if (getinfo(&ifreq) < 0)
				continue;
			status(1);
			count++;
			noinet = 1;
			continue;
		}
		if (!strncmp(ifreq.ifr_name, ifrp->ifr_name,
		    sizeof(ifrp->ifr_name))) {
			register struct afswtch *p = afp;

			if (ifaliases == 0 && noinet == 0)
				continue;
			ifr = *ifrp;
			if ((p = afp) != NULL) {
				(*p->af_status)(1);
			} else for (p = afs; p->af_name; p++) {
				ifr.ifr_addr.sa_family = p->af_af;
				(*p->af_status)(0);
			}
			count++;
			noinet = 0;
			continue;
		}
	}
	free(inbuf);
	if (count == 0) {
		fprintf(stderr, "%s: no such interface\n", name);
		exit(1);
	}
}

#define RIDADDR 0
#define ADDR	1
#define MASK	2
#define DSTADDR	3

/*ARGSUSED*/
void
setifaddr(addr, param)
	char *addr;
	int param;
{
	/*
	 * Delay the ioctl to set the interface addr until flags are all set.
	 * The address interpretation may depend on the flags,
	 * and the flags may change when the address is set.
	 */
	setaddr++;
	if (doalias == 0)
		clearaddr = 1;
	(*afp->af_getaddr)(addr, (doalias >= 0 ? ADDR : RIDADDR));
}

void
setifnetmask(addr)
	char *addr;
{
	(*afp->af_getaddr)(addr, MASK);
}

void
setifbroadaddr(addr)
	char *addr;
{
	(*afp->af_getaddr)(addr, DSTADDR);
}

void
setifipdst(addr)
	char *addr;
{
	in_getaddr(addr, DSTADDR);
	setipdst++;
	clearaddr = 0;
	newaddr = 0;
}

#define rqtosa(x) (&(((struct ifreq *)(afp->x))->ifr_addr))
/*ARGSUSED*/
void
notealias(addr, param)
	char *addr;
	int param;
{
	if (setaddr && doalias == 0 && param < 0)
		memcpy(rqtosa(af_ridreq), rqtosa(af_addreq),
		    rqtosa(af_addreq)->sa_len);
	doalias = param;
	if (param < 0) {
		clearaddr = 1;
		newaddr = 0;
	} else
		clearaddr = 0;
}

/*ARGSUSED*/
void
notrailers(vname, value)
	char *vname;
	int value;
{
	printf("Note: trailers are no longer sent, but always received\n");
}

/*ARGSUSED*/
void
setifdstaddr(addr, param)
	char *addr;
	int param;
{
	(*afp->af_getaddr)(addr, DSTADDR);
}

void
setifflags(vname, value)
	char *vname;
	int value;
{
 	if (ioctl(s, SIOCGIFFLAGS, (caddr_t)&ifr) < 0)
		err(1, "SIOCGIFFLAGS");
	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
 	flags = ifr.ifr_flags;

	if (value < 0) {
		value = -value;
		flags &= ~value;
	} else
		flags |= value;
	ifr.ifr_flags = flags;
	if (ioctl(s, SIOCSIFFLAGS, (caddr_t)&ifr) < 0)
		err(1, "SIOCSIFFLAGS");
}

void
setifmetric(val)
	char *val;
{
	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
	ifr.ifr_metric = atoi(val);
	if (ioctl(s, SIOCSIFMETRIC, (caddr_t)&ifr) < 0)
		warn("SIOCSIFMETRIC");
}

#define	IFFBITS \
"\020\1UP\2BROADCAST\3DEBUG\4LOOPBACK\5POINTOPOINT\6NOTRAILERS\7RUNNING\10NOARP\
\11PROMISC\12ALLMULTI\13OACTIVE\14SIMPLEX\15LINK0\16LINK1\17LINK2\20MULTICAST"

/*
 * Print the status of the interface.  If an address family was
 * specified, show it and it only; otherwise, show them all.
 */
void
status(link)
	int link;
{
	register struct afswtch *p = afp;

	printf("%s: ", name);
	printb("flags", flags, IFFBITS);
	if (metric)
		printf(" metric %d", metric);
	putchar('\n');
	if (link == 0) {
		if ((p = afp) != NULL) {
			(*p->af_status)(1);
		} else for (p = afs; p->af_name; p++) {
			ifr.ifr_addr.sa_family = p->af_af;
			(*p->af_status)(0);
		}
	}
}

void
in_status(force)
	int force;
{
	struct sockaddr_in *sin, sin2;
	char *inet_ntoa();

	getsock(AF_INET);
	if (s < 0) {
		if (errno == EPROTONOSUPPORT)
			return;
		err(1, "socket");
	}
	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
	sin = (struct sockaddr_in *)&ifr.ifr_addr;

	/*
	 * We keep the interface address and reset it before each
	 * ioctl() so we can get ifaliases information (as opposed
 	 * to the primary interface netmask/dstaddr/broadaddr, if
	 * the ifr_addr field is zero).
	 */
	memcpy(&sin2, &ifr.ifr_addr, sizeof(sin2));

	printf("\tinet %s ", inet_ntoa(sin->sin_addr));
	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
	if (ioctl(s, SIOCGIFNETMASK, (caddr_t)&ifr) < 0) {
		if (errno != EADDRNOTAVAIL)
			warn("SIOCGIFNETMASK");
		memset(&ifr.ifr_addr, 0, sizeof(ifr.ifr_addr));
	} else
		netmask.sin_addr =
		    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
	if (flags & IFF_POINTOPOINT) {
		memcpy(&ifr.ifr_addr, &sin2, sizeof(sin2));
		if (ioctl(s, SIOCGIFDSTADDR, (caddr_t)&ifr) < 0) {
			if (errno == EADDRNOTAVAIL)
			    memset(&ifr.ifr_addr, 0, sizeof(ifr.ifr_addr));
			else
			    warn("SIOCGIFDSTADDR");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		sin = (struct sockaddr_in *)&ifr.ifr_dstaddr;
		printf("--> %s ", inet_ntoa(sin->sin_addr));
	}
	printf("netmask 0x%x ", ntohl(netmask.sin_addr.s_addr));
	if (flags & IFF_BROADCAST) {
		memcpy(&ifr.ifr_addr, &sin2, sizeof(sin2));
		if (ioctl(s, SIOCGIFBRDADDR, (caddr_t)&ifr) < 0) {
			if (errno == EADDRNOTAVAIL)
			    memset(&ifr.ifr_addr, 0, sizeof(ifr.ifr_addr));
			else
			    warn("SIOCGIFBRDADDR");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		sin = (struct sockaddr_in *)&ifr.ifr_addr;
		if (sin->sin_addr.s_addr != 0)
			printf("broadcast %s", inet_ntoa(sin->sin_addr));
	}
	putchar('\n');
}

#ifndef INET_ONLY

void
at_status(force)
	int force;
{
	struct sockaddr_at *sat, null_sat;
	struct netrange *nr;
 
	getsock(AF_APPLETALK);
	if (s < 0) {
		if (errno == EPROTONOSUPPORT)
			return;
		err(1, "socket");
	}
	(void) memset(&ifr, 0, sizeof(ifr));
	(void) strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	if (ioctl(s, SIOCGIFADDR, (caddr_t)&ifr) < 0) {
		if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
			if (!force)
				return;
			(void) memset(&ifr.ifr_addr, 0, sizeof(ifr.ifr_addr));
		} else
			warn("SIOCGIFADDR");
	}
	(void) strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
	sat = (struct sockaddr_at *)&ifr.ifr_addr;

	(void) memset(&null_sat, 0, sizeof(null_sat));

	nr = (struct netrange *) &sat->sat_zero;
	printf("\tAppleTalk %d.%d range %d-%d phase %d",
	    ntohs(sat->sat_addr.s_net), sat->sat_addr.s_node,
	    ntohs(nr->nr_firstnet), ntohs(nr->nr_lastnet), nr->nr_phase);
	if (flags & IFF_POINTOPOINT) {  
		if (ioctl(s, SIOCGIFDSTADDR, (caddr_t)&ifr) < 0) {
			if (errno == EADDRNOTAVAIL)
			    (void) memset(&ifr.ifr_addr, 0,
				sizeof(ifr.ifr_addr));
			else
			    warn("SIOCGIFDSTADDR");
		}
		(void) strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		sat = (struct sockaddr_at *)&ifr.ifr_dstaddr;
		if (!sat)
			sat = &null_sat;
		printf("--> %d.%d",
		    ntohs(sat->sat_addr.s_net), sat->sat_addr.s_node);
	}
	if (flags & IFF_BROADCAST) {
		/* note RTAX_BRD overlap with IFF_POINTOPOINT */
		sat = (struct sockaddr_at *)&ifr.ifr_broadaddr;
		if (sat)
			printf(" broadcast %d.%d", ntohs(sat->sat_addr.s_net),
			    sat->sat_addr.s_node);
	}
	putchar('\n');
}

void
xns_status(force)
	int force;
{
	struct sockaddr_ns *sns;

	getsock(AF_NS);
	if (s < 0) {
		if (errno == EPROTONOSUPPORT)
			return;
		err(1, "socket");
	}
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	if (ioctl(s, SIOCGIFADDR, (caddr_t)&ifr) < 0) {
		if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
			if (!force)
				return;
			memset(&ifr.ifr_addr, 0, sizeof(ifr.ifr_addr));
		} else
			warn("SIOCGIFADDR");
	}
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
	sns = (struct sockaddr_ns *)&ifr.ifr_addr;
	printf("\tns %s ", ns_ntoa(sns->sns_addr));
	if (flags & IFF_POINTOPOINT) { /* by W. Nesheim@Cornell */
		if (ioctl(s, SIOCGIFDSTADDR, (caddr_t)&ifr) < 0) {
			if (errno == EADDRNOTAVAIL)
			    memset(&ifr.ifr_addr, 0, sizeof(ifr.ifr_addr));
			else
			    warn("SIOCGIFDSTADDR");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		sns = (struct sockaddr_ns *)&ifr.ifr_dstaddr;
		printf("--> %s ", ns_ntoa(sns->sns_addr));
	}
	putchar('\n');
}

void
setipxframetype(vname, type)
	char	*vname;
	int	type;
{
	ipx_type = type;
}

void
ipx_status(force)
	int force;
{
	struct sockaddr_ipx *sipx;

	getsock(AF_IPX);
	if (s < 0) {
		if (errno == EPROTONOSUPPORT)
			return;
		err(1, "socket");
	}
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	if (ioctl(s, SIOCGIFADDR, (caddr_t)&ifr) < 0) {
		if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
			if (!force)
				return;
			memset(&ifr.ifr_addr, 0, sizeof(ifr.ifr_addr));
		} else
			warn("SIOCGIFADDR");
	}
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
	sipx = (struct sockaddr_ipx *)&ifr.ifr_addr;
	printf("\tipx %s ", ipx_ntoa(sipx->sipx_addr));
	if (flags & IFF_POINTOPOINT) { /* by W. Nesheim@Cornell */
		if (ioctl(s, SIOCGIFDSTADDR, (caddr_t)&ifr) < 0) {
			if (errno == EADDRNOTAVAIL)
			    memset(&ifr.ifr_addr, 0, sizeof(ifr.ifr_addr));
			else
			    warn("SIOCGIFDSTADDR");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		sipx = (struct sockaddr_ipx *)&ifr.ifr_dstaddr;
		printf("--> %s ", ipx_ntoa(sipx->sipx_addr));
	}
	{
		struct frame_types {
			int	type;
			char	*name;
		} *p, frames[] = {
			{ ETHERTYPE_8022, "802.2" },
			{ ETHERTYPE_8022TR, "802.2tr" },
			{ ETHERTYPE_8023, "802.3" },
			{ ETHERTYPE_SNAP, "SNAP" },
			{ ETHERTYPE_II,  "EtherII" },
			{ 0, NULL }
		};
		for (p = frames; p->name && p->type != sipx->sipx_type; p++);
		if (p->name != NULL)
			printf("frame %s ", p->name);
	}
	putchar('\n');
}

void
iso_status(force)
	int force;
{
	struct sockaddr_iso *siso;
	struct iso_ifreq ifr;

	getsock(AF_ISO);
	if (s < 0) {
		if (errno == EPROTONOSUPPORT)
			return;
		err(1, "socket");
	}
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	if (ioctl(s, SIOCGIFADDR_ISO, (caddr_t)&ifr) < 0) {
		if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
			if (!force)
				return;
			memset(&ifr.ifr_Addr, 0, sizeof(ifr.ifr_Addr));
		} else
			warn("SIOCGIFADDR_ISO");
	}
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
	siso = &ifr.ifr_Addr;
	printf("\tiso %s ", iso_ntoa(&siso->siso_addr));
	if (ioctl(s, SIOCGIFNETMASK_ISO, (caddr_t)&ifr) < 0) {
		if (errno == EADDRNOTAVAIL)
			memset(&ifr.ifr_Addr, 0, sizeof(ifr.ifr_Addr));
		else
			warn("SIOCGIFNETMASK_ISO");
	} else {
		printf(" netmask %s ", iso_ntoa(&siso->siso_addr));
	}
	if (flags & IFF_POINTOPOINT) {
		if (ioctl(s, SIOCGIFDSTADDR_ISO, (caddr_t)&ifr) < 0) {
			if (errno == EADDRNOTAVAIL)
			    memset(&ifr.ifr_Addr, 0, sizeof(ifr.ifr_Addr));
			else
			    warn("SIOCGIFDSTADDR_ISO");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		siso = &ifr.ifr_Addr;
		printf("--> %s ", iso_ntoa(&siso->siso_addr));
	}
	putchar('\n');
}

#endif	/* INET_ONLY */

struct	in_addr inet_makeaddr();

#define SIN(x) ((struct sockaddr_in *) &(x))
struct sockaddr_in *sintab[] = {
SIN(ridreq.ifr_addr), SIN(addreq.ifra_addr),
SIN(addreq.ifra_mask), SIN(addreq.ifra_broadaddr)};

void
in_getaddr(s, which)
	char *s;
	int which;
{
	register struct sockaddr_in *sin = sintab[which];
	struct hostent *hp;
	struct netent *np;

	sin->sin_len = sizeof(*sin);
	if (which != MASK)
		sin->sin_family = AF_INET;

	if (inet_aton(s, &sin->sin_addr) == 0) {
		if ((hp = gethostbyname(s)))
			memcpy(&sin->sin_addr, hp->h_addr, hp->h_length);
		else if ((np = getnetbyname(s)))
			sin->sin_addr = inet_makeaddr(np->n_net, INADDR_ANY);
		else
			errx(1, "%s: bad value", s);
	}
}

/*
 * Print a value a la the %b format of the kernel's printf
 */
void
printb(s, v, bits)
	char *s;
	register char *bits;
	register unsigned short v;
{
	register int i, any = 0;
	register char c;

	if (bits && *bits == 8)
		printf("%s=%o", s, v);
	else
		printf("%s=%x", s, v);
	bits++;
	if (bits) {
		putchar('<');
		while ((i = *bits++)) {
			if (v & (1 << (i-1))) {
				if (any)
					putchar(',');
				any = 1;
				for (; (c = *bits) > 32; bits++)
					putchar(c);
			} else
				for (; *bits > 32; bits++)
					;
		}
		putchar('>');
	}
}

#ifndef INET_ONLY

void
at_getaddr(addr, which)
	char *addr;
	int which;
{
	struct sockaddr_at *sat = (struct sockaddr_at *) &addreq.ifra_addr;
	u_int net, node;

	sat->sat_family = AF_APPLETALK;
	sat->sat_len = sizeof(*sat);    
	if (which == MASK)
		errx(1, "AppleTalk does not use netmasks");
	if (sscanf(addr, "%u.%u", &net, &node) != 2 ||
	    net == 0 || net > 0xffff || node == 0 || node > 0xfe)
		errx(1, "%s: illegal address", addr);
	sat->sat_addr.s_net = htons(net);
	sat->sat_addr.s_node = node;
}
	
void
setatrange(range, d)
	char *range;
	int d;
{
	u_short first = 123, last = 123;
	
	if (sscanf(range, "%hu-%hu", &first, &last) != 2 ||
	    first == 0 || first > 0xffff ||
	    last == 0 || last > 0xffff || first > last)
		errx(1, "%s: illegal net range: %u-%u", range, first, last);
	at_nr.nr_firstnet = htons(first);
	at_nr.nr_lastnet = htons(last);
}
 
void
setatphase(phase, d)
	char *phase;
	int d;
{
	if (!strcmp(phase, "1"))
		at_nr.nr_phase = 1;
	else if (!strcmp(phase, "2"))  
		at_nr.nr_phase = 2;
	else
		errx(1, "%s: illegal phase", phase);
}
	
void
checkatrange(sat)
	struct sockaddr_at *sat;
{
	if (at_nr.nr_phase == 0)
		at_nr.nr_phase = 2;     /* Default phase 2 */
	if (at_nr.nr_firstnet == 0)	/* Default range of one */
		at_nr.nr_firstnet = at_nr.nr_lastnet = sat->sat_addr.s_net;
	printf("\tatalk %d.%d range %d-%d phase %d\n",
	ntohs(sat->sat_addr.s_net), sat->sat_addr.s_node,
	ntohs(at_nr.nr_firstnet), ntohs(at_nr.nr_lastnet), at_nr.nr_phase);
	if ((u_short) ntohs(at_nr.nr_firstnet) >
	    (u_short) ntohs(sat->sat_addr.s_net) ||
	    (u_short) ntohs(at_nr.nr_lastnet) <
	    (u_short) ntohs(sat->sat_addr.s_net))
		errx(1, "AppleTalk address is not in range");
	*((struct netrange *) &sat->sat_zero) = at_nr;
}

#define SNS(x) ((struct sockaddr_ns *) &(x))
struct sockaddr_ns *snstab[] = {
SNS(ridreq.ifr_addr), SNS(addreq.ifra_addr),
SNS(addreq.ifra_mask), SNS(addreq.ifra_broadaddr)};

void
xns_getaddr(addr, which)
	char *addr;
	int which;
{
	struct sockaddr_ns *sns = snstab[which];
	struct ns_addr ns_addr();

	sns->sns_family = AF_NS;
	sns->sns_len = sizeof(*sns);
	sns->sns_addr = ns_addr(addr);
	if (which == MASK)
		printf("Attempt to set XNS netmask will be ineffectual\n");
}

#define SIPX(x) ((struct sockaddr_ipx *) &(x))
struct sockaddr_ipx *sipxtab[] = {
SIPX(ridreq.ifr_addr), SIPX(addreq.ifra_addr),
SIPX(addreq.ifra_mask), SIPX(addreq.ifra_broadaddr)};

void
ipx_getaddr(addr, which)
	char *addr;
	int which;
{
	struct sockaddr_ipx *sipx = sipxtab[which];
	struct ipx_addr ipx_addr();

	sipx->sipx_family = AF_IPX;
	sipx->sipx_len  = sizeof(*sipx);
	sipx->sipx_addr = ipx_addr(addr);
	sipx->sipx_type = ipx_type;
	if (which == MASK)
		printf("Attempt to set IPX netmask will be ineffectual\n");
}

#define SISO(x) ((struct sockaddr_iso *) &(x))
struct sockaddr_iso *sisotab[] = {
SISO(iso_ridreq.ifr_Addr), SISO(iso_addreq.ifra_addr),
SISO(iso_addreq.ifra_mask), SISO(iso_addreq.ifra_dstaddr)};

void
iso_getaddr(addr, which)
	char *addr;
	int which;
{
	register struct sockaddr_iso *siso = sisotab[which];
	struct iso_addr *iso_addr();
	siso->siso_addr = *iso_addr(addr);

	if (which == MASK) {
		siso->siso_len = TSEL(siso) - (caddr_t)(siso);
		siso->siso_nlen = 0;
	} else {
		siso->siso_len = sizeof(*siso);
		siso->siso_family = AF_ISO;
	}
}

void
setsnpaoffset(val)
	char *val;
{
	iso_addreq.ifra_snpaoffset = atoi(val);
}

void
setnsellength(val)
	char *val;
{
	nsellength = atoi(val);
	if (nsellength < 0)
		errx(1, "negative NSEL length is absurd");
	if (afp == 0 || afp->af_af != AF_ISO)
		errx(1, "setting NSEL length valid only for iso");
}

void
fixnsel(s)
	register struct sockaddr_iso *s;
{
	if (s->siso_family == 0)
		return;
	s->siso_tlen = nsellength;
}

void
adjust_nsellength()
{
	fixnsel(sisotab[RIDADDR]);
	fixnsel(sisotab[ADDR]);
	fixnsel(sisotab[DSTADDR]);
}

#endif	/* INET_ONLY */

void
usage()
{
	fprintf(stderr, "usage: ifconfig interface\n%s",
		"\t[ [af] [ address [ dest_addr ] ] [ up ] [ down ] "
		"[ netmask mask ] ]\n"
		"\t[ metric n ]\n"
		"\t[ arp | -arp ]\n"
		"\t[ -802.2 | -802.3 | -802.2tr | -snap | -EtherII ]\n"
		"\t[ link0 | -link0 ] [ link1 | -link1 ] [ link2 | -link2 ]\n"
		"       ifconfig [-a | -A] [ af ]\n");
	exit(1);
}
