#	$OpenBSD: src/lib/libc/Makefile,v 1.32 2013/06/01 21:26:17 stsp Exp $
#
# The NLS (message catalog) functions are always in libc.  To choose that
# strerror(), perror(), strsignal(), psignal(), etc. actually call the NLS
# functions, put -DNLS on the CFLAGS line below.
#
# The YP functions are always in libc. To choose that getpwent() and friends
# actually call the YP functions, put -DYP on the CFLAGS line below.

.include <bsd.own.mk>

LIB=c
CLEANFILES+=tags
#CFLAGS+=-Werror
LDADD=-nodefaultlibs -lgcc

LIBCSRCDIR=${.CURDIR}
.include "${LIBCSRCDIR}/Makefile.inc"

NLS= 	C.msg Pig.msg da.ISO8859-1.msg da.UTF-8.msg de.ISO8859-1.msg \
	de.UTF-8.msg es.ISO8859-1.msg es.UTF-8.msg fi.ISO8859-1.msg \
	fi.UTF-8.msg fr.ISO8859-1.msg fr.UTF-8.msg it.UTF-8.msg \
	nl.ISO8859-1.msg nl.UTF-8.msg no.ISO8859-1.msg no.UTF-8.msg \
	ru.KOI8-R.msg ru.UTF-8.msg sv.ISO8859-1.msg sv.UTF-8.msg

copy-to-libkern:	copy-to-libkern-machind copy-to-libkern-machdep

.if make(copy-to-libkern)
copy-to-libkern-machind: ${KSRCS}
	cp -p ${.ALLSRC} ${LIBKERN}
.if defined(KINCLUDES) && !empty(KINCLUDES)
	(cd ${.CURDIR} ; cp -p ${KINCLUDES} ${LIBKERN})
.endif

copy-to-libkern-machdep: ${KMSRCS}
.if defined(KMSRCS) && !empty(KMSRCS)
	cp -p ${.ALLSRC} ${LIBKERN}/arch/${MACHINE_CPU}
.endif
.if defined(KMINCLUDES) && !empty(KMINCLUDES)
	(cd ${.CURDIR} ; cp -p ${KMINCLUDES} ${LIBKERN}/arch/${MACHINE_CPU})
.endif

rm-from-libkern:
	for i in ${KSRCS}; do rm -f ${LIBKERN}/$$i; done
.if defined(KMSRCS) && !empty(KMSRCS)
	for i in ${KMSRCS}; do rm -f ${LIBKERN}/arch/${MACHINE_CPU}/$$i; done
.endif
.endif

all: tags
tags: ${SRCS}
	ctags -w ${.ALLSRC:M*.c}
	egrep "^SYSENTRY(.*)|^ENTRY(.*)|^FUNC(.*)|^SYSCALL(.*)" \
	    /dev/null ${.ALLSRC:M*.S} | \
	    sed "s;\([^:]*\):\([^(]*\)(\([^, )]*\)\(.*\);\3	\1	/^\2(\3\4$$/;" \
	    >> tags; sort -o tags tags

beforeinstall:
	${INSTALL} ${INSTALL_COPY} -o ${BINOWN} -g ${BINGRP} -m 444 tags \
		${DESTDIR}/var/db/lib${LIB}.tags

.include <bsd.lib.mk>
