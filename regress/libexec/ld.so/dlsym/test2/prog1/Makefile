# $OpenBSD: src/regress/libexec/ld.so/dlsym/test2/prog1/Makefile,v 1.3 2005/09/19 01:20:58 kurt Exp $

PROG=		prog1
SRCS=		main.c
LDADD+=		-laa
LDFLAGS+=	-Wl,-E
LDFLAGS+=	-Wl,-rpath,$(AA_OBJDIR)
LDFLAGS+=	-Wl,-rpath,$(BB_OBJDIR)
LDFLAGS+=	-L$(AA_OBJDIR)

.include <bsd.regress.mk>
