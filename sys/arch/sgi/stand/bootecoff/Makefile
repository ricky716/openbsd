#	$OpenBSD: src/sys/arch/sgi/stand/bootecoff/Makefile,v 1.1 2012/03/19 17:38:31 miod Exp $

.include "${.CURDIR}/../Makefile32.inc"
STRIP?=		strip
LDSCRIPT=	${.CURDIR}/ld.script
LDFLAGS+=	${SALDFLAGS} -T ${LDSCRIPT} -e __start -N -s
PROG=		bootecoff
.PATH:	${.CURDIR}/../boot
.include "${.CURDIR}/../boot/Makefile"

SRCS+=	ashrdi3.c moddi3.c udivdi3.c umoddi3.c qdivrem.c

${PROG}: $(OBJS) $(LDADD)
	$(LD) $(LDFLAGS) -o ${PROG} $(OBJS) -L${LIBSADIR} ${LIBSA} \
	    -L${LIBZDIR} ${LIBZ}

LINKS=		${BINDIR}/${PROG} ${BINDIR}/boot-IP22

.include <bsd.prog.mk>
