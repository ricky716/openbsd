#	$OpenBSD: src/usr.bin/passwd/Makefile,v 1.38 2014/04/22 10:21:56 reyk Exp $

.include <bsd.own.mk>

PROG=	passwd
SRCS=	local_passwd.c yp_passwd.c passwd.c pwd_gensalt.c getpwent.c \
	pwd_check.c
.PATH:  ${.CURDIR}/../../lib/libc/gen
DPADD+= ${LIBRPCSVC} ${LIBUTIL}
LDADD+= -lrpcsvc -lutil
CFLAGS+= -I${.CURDIR}

CFLAGS+=-I${.CURDIR}/../../lib/libc/include
.if (${YP:L} == "yes")
CFLAGS+=-DYP -I${.CURDIR}/../../lib/libc/yp
.endif

BINMODE=4555
BINOWN=root

getpwent.o: getpwent.c
	${COMPILE.c} -UYP ${.CURDIR}/../../lib/libc/gen/getpwent.c

.include <bsd.prog.mk>
