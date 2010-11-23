lib_LIBRARIES=libomalloc.a libomalloc_debug.a

noinst_HEADERS=\
mylimits.h             omlimits.h \
om_Alloc.h             omList.h  	      omDerivedConfig.h \
omAllocDecl.h          omConfig.h             omMemOps.h	\
omDebug.h              omOpts.h		      omInlineDecl.h    \
omAllocPrivate.h       omError.h              omPage.h		\
omAllocSystem.h        omFindExec.h           omStats.h		\
omBin.h                omDefaultConfig.h      omStructs.h	\
omBinPage.h            omInline.h             omGetBackTrace.h  \
omReturn.h	       omRet2Info.h	      omAllocFunc.h     \
omTables.h

SOURCES=\
omBinPage.c       omList.c         omAllocEmulate.c omDebug.c        \
om_Alloc.c        omDebugCheck.c   omOpts.c         omGetBackTrace.c \
omAllocSystem.c   omError.c        omStats.c	    omRet2Info.c     \
omBin.c           omFindExec.c     omDebugTrack.c                    \
omalloc_provide.c omAllocFunc.c

INCLUDES=-I${top_srcdir}/.. -I${top_builddir}/..

libomalloc_a_SOURCES=$(SOURCES) $(noinst_HEADERS)
libomalloc_debug_a_SOURCES=$(SOURCES) $(noinst_HEADERS)

nodist_libomalloc_a_SOURCES = omConfig.h omlimits.h mylimits.h omTables.h omalloc.h omTables.inc
nodist_libomalloc_debug_a_SOURCES = omConfig.h omlimits.h omTables.h omalloc.h omTables.inc

libomalloc_a_CPPFLAGS= -DOM_NDEBUG

BUILT_SOURCES = omalloc.h omTables.inc

omalloc.h: $(noinst_HEADERS) omTables.h
	./makeheader om_Alloc.h $@

omTables.inc: omTables omalloc.h
	./omTables > omTables.xx && mv omTables.xx  $@

omTables.h: omTables
	./omTables 1 >omTables.yy && mv omTables.yy $@

noinst_PROGRAMS = omTables
omTables_SOURCES = omAllocPrivate.h omConfig.h omTables.c omlimits.h mylimits.h
omTables_CPPFLAGS = -DOM_GENERATE_INC

