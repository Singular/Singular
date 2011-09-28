ACLOCAL_AMFLAGS = -I ../m4
AM_LDFLAGS = -L${top_builddir}/omalloc -L${top_builddir}/../omalloc

lib_LIBRARIES=libomalloc.a libomalloc_g.a

libomalloc_includedir=$(includedir)/omalloc

libomalloc_include_HEADERS = \
omalloc.h		omConfig.h		omDerivedConfig.h	\
omError.h		omStructs.h		omAllocDecl.h		\
omInlineDecl.h		omBin.h			omMemOps.h		\
omList.h		omFindExec.h		omGetBackTrace.h	\
omRet2Info.h		omStats.h		omOpts.h		\
omBinPage.h		omAllocSystem.h		omTables.h		\
omAllocPrivate.h	omDebug.h		omInline.h		\
omAllocFunc.h 		omlimits.h 		mylimits.h

noinst_HEADERS= omPage.h omDefaultConfig.h omReturn.h omGetPageSize.h \
  omMalloc.h omMallocSystem.h

SOURCES=\
omBinPage.c       omList.c         omAllocEmulate.c omDebug.c        \
om_Alloc.c        omDebugCheck.c   omOpts.c         omGetBackTrace.c \
omAllocSystem.c   omError.c        omStats.c	    omRet2Info.c     \
omBin.c           omFindExec.c     omDebugTrack.c   		     \
omalloc_provide.c omAllocFunc.c

EXTRA_DIST = omalloc.c omtTestAlloc.c omtTest.h

INCLUDES=-I${top_srcdir}/.. -I${top_builddir}/..

libomalloc_a_SOURCES=$(SOURCES) $(noinst_HEADERS)
libomalloc_g_a_SOURCES=$(SOURCES) $(noinst_HEADERS)

nodist_libomalloc_a_SOURCES = omConfig.h omlimits.h mylimits.h omalloc.h omTables.inc
nodist_libomalloc_g_a_SOURCES = omConfig.h omlimits.h omalloc.h omTables.inc

libomalloc_a_CPPFLAGS= -DOM_NDEBUG

BUILT_SOURCES = omTables.inc omTables.h

omTables.inc: omTables omalloc.h
	./omTables > omTables.xx && mv omTables.xx  $@

omTables.h: omTables
	./omTables 1 >omTables.yy && mv omTables.yy $@

noinst_PROGRAMS = omTables
omTables_SOURCES = omAllocPrivate.h omConfig.h omTables.c omlimits.h mylimits.h
omTables_CPPFLAGS = -DOM_GENERATE_INC

CLEANFILES = $(BUILT_SOURCES)

####################################################
## Test program
##
TESTS = omtTest-g omtTest-m omtTest-r
check_PROGRAMS = $(TESTS)

OMTTESTSSOURCES = omtTestReal.c omtTestDebug.c omtTestKeep.c omtTestError.c omtTest.c

omtTest_g: libomalloc_g.a

omtTest_g_SOURCES = $(OMTTESTSSOURCES)
omtTest_g_CFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long
omtTest_g_LDADD = -lomalloc_g

omtTest_m: libomalloc_g.a

omtTest_m_CPPFLAGS = -DOM_TEST_MALLOC
omtTest_m_SOURCES  = $(OMTTESTSSOURCES)
omtTest_m_LDADD = -lomalloc_g

omtTest_r: libomalloc.a

omtTest_r_SOURCES = $(OMTTESTSSOURCES)
omtTest_r_CPPFLAGS = -DOM_NDEBUG
omtTest_r_CFLAGS = -O3
omtTest_r_LDADD = -lomalloc

