#	$OpenBSD: src/usr.bin/pkill/Makefile,v 1.1 2004/01/06 20:07:49 millert Exp $
#	$NetBSD: Makefile,v 1.1 2002/03/01 11:21:58 ad Exp $

PROG=	pkill
SRCS=	pkill.c
MAN=	pkill.1

LDADD+=	-lkvm
DPADD+=	${LIBKVM}

LINKS+=	${BINDIR}/pkill ${BINDIR}/pgrep
MLINKS+=pkill.1 pgrep.1

.include <bsd.prog.mk>
