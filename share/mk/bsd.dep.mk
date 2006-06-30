#	$OpenBSD: src/share/mk/bsd.dep.mk,v 1.6 2006/06/30 19:00:29 otto Exp $
#	$NetBSD: bsd.dep.mk,v 1.12 1995/09/27 01:15:09 christos Exp $

# some of the rules involve .h sources, so remove them from mkdep line
.if !target(depend)
depend: beforedepend .depend _SUBDIRUSE afterdepend
.  if defined(SRCS)
.depend: ${SRCS} ${_LEXINTM} ${_YACCINTM}
	@rm -f .depend
	@files="${.ALLSRC:M*.s} ${.ALLSRC:M*.S}"; \
	if [ "$$files" != " " ]; then \
	  echo mkdep -a ${MKDEP} ${CFLAGS:M-[ID]*} ${CPPFLAGS} ${AINC} $$files;\
	  mkdep -a ${MKDEP} ${CFLAGS:M-[ID]*} ${CPPFLAGS} ${AINC} $$files; \
	fi
	@files="${.ALLSRC:M*.c}"; \
	if [ "$$files" != "" ]; then \
	  echo mkdep -a ${MKDEP} ${CFLAGS:M-[ID]*} ${CPPFLAGS} $$files; \
	  mkdep -a ${MKDEP} ${CFLAGS:M-[ID]*} ${CPPFLAGS} $$files; \
	fi
	@files="${.ALLSRC:M*.cc} ${.ALLSRC:M*.C} ${.ALLSRC:M*.cxx}"; \
	if [ "$$files" != "  " ]; then \
	  echo mkdep -a ${MKDEP} ${CXXFLAGS:M-[ID]*} ${CPPFLAGS} $$files; \
	  mkdep -a ${MKDEP} ${CXXFLAGS:M-[ID]*} ${CPPFLAGS} $$files; \
	fi
.  else
.depend:
.  endif
.  if !target(beforedepend)
beforedepend:
.  endif
.  if !target(afterdepend)
afterdepend:
.  endif
.endif

.if !target(tags)
.  if defined(SRCS)
tags: ${SRCS} _SUBDIRUSE
	-cd ${.CURDIR}; ${CTAGS} -f /dev/stdout ${.ALLSRC:N*.h} | \
	    sed "s;\${.CURDIR}/;;" > tags
.  else
tags:
.  endif
.endif

.if defined(SRCS)
cleandir: cleandepend
cleandepend:
	rm -f .depend ${.CURDIR}/tags
.endif

.PHONY: beforedepend depend afterdepend cleandepend
