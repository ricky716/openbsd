# $OpenBSD: src/sbin/iked/Makefile,v 1.11 2014/02/17 15:07:23 markus Exp $

PROG=		iked
SRCS=		ca.c chap_ms.c config.c control.c crypto.c dh.c \
		eap.c iked.c ikev1.c ikev2.c ikev2_msg.c ikev2_pld.c \
		log.c ocsp.c pfkey.c policy.c proc.c timer.c util.c imsg_util.c
SRCS+=		eap_map.c ikev2_map.c
SRCS+=		parse.y
MAN=		iked.conf.5 iked.8
#NOMAN=		yes

LDADD=		-lutil -levent -lcrypto
DPADD=		${LIBUTIL} ${LIBEVENT} ${LIBCRYPTO}
CFLAGS+=	-Wall -I${.CURDIR}
CFLAGS+=	-Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=	-Wmissing-declarations
CFLAGS+=	-Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+=	-Wsign-compare
CLEANFILES+=	y.tab.h ikev2_map.c eap_map.c
GENERATED=	ikev2_map.c eap_map.c

ikev2_map.c: genmap.sh ikev2.h
	/bin/sh ${.CURDIR}/genmap.sh ${.CURDIR}/ikev2.h ikev2 > $@
	@touch $@

eap_map.c: genmap.sh eap.h
	/bin/sh ${.CURDIR}/genmap.sh ${.CURDIR}/eap.h eap > $@
	@touch $@

${PROG} beforedepend: ${GENERATED}

.include <bsd.prog.mk>
