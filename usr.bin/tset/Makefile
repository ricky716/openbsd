#	$OpenBSD: src/usr.bin/tset/Makefile,v 1.9 2013/09/18 16:21:29 millert Exp $

PROG=	tset
SRCS=	tset.c dump_entry.c
DPADD=	${LIBCURSES}
LDADD=	-lcurses
CURSES= ${.CURDIR}/../../lib/libcurses
TIC= ${.CURDIR}/../tic
CFLAGS+= -I${CURSES} -I${TIC} -I${.CURDIR} -I. -DHAVE_GETTTYNAM -DHAVE_TTYENT_H
CLEANFILES+= termsort.c
.PATH:  ${TIC}
LINKS=	${BINDIR}/tset ${BINDIR}/reset
MLINKS=	tset.1 reset.1

beforedepend: termsort.c

termsort.c: ${TIC}/MKtermsort.sh
	sh ${TIC}/MKtermsort.sh awk ${CURSES}/Caps > ${.TARGET}

.include <bsd.prog.mk>
