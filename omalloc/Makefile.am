ACLOCAL_AMFLAGS = -I ../m4

lib_LTLIBRARIES=libomalloc.la libomalloc_g.la

libomalloc_includedir=$(includedir)/omalloc

libomalloc_include_HEADERS = \
omalloc.h		omConfig.h		omDerivedConfig.h	\
omError.h		omStructs.h		omAllocDecl.h		\
omInlineDecl.h		omBin.h			omMemOps.h		\
omList.h		omFindExec.h		omGetBackTrace.h	\
omRet2Info.h		omStats.h		omOpts.h		\
omBinPage.h		omAllocSystem.h		omTables.h		\
omAllocPrivate.h	omDebug.h		omInline.h		\
omAllocFunc.h 		mylimits.h

noinst_HEADERS= omPage.h omDefaultConfig.h omReturn.h omGetPageSize.h \
  omMalloc.h omMallocSystem.h

SOURCES=\
omBinPage.c       omList.c         omAllocEmulate.c omDebug.c        \
om_Alloc.c        omDebugCheck.c   omOpts.c         omGetBackTrace.c \
omAllocSystem.c   omError.c        omStats.c	    omRet2Info.c     \
omBin.c           omFindExec.c     omDebugTrack.c   		     \
omalloc_provide.c omAllocFunc.c

EXTRA_DIST = omalloc.c omtTestAlloc.c omtTest.h omMmap.c

INCLUDES=-I${abs_top_srcdir}/.. -I${abs_top_builddir}/..

libomalloc_la_SOURCES=$(SOURCES) $(noinst_HEADERS)
libomalloc_g_la_SOURCES=$(SOURCES) $(noinst_HEADERS)

nodist_libomalloc_la_SOURCES = omConfig.h mylimits.h omalloc.h omTables.inc
nodist_libomalloc_g_la_SOURCES = omConfig.h omalloc.h omTables.inc

libomalloc_la_CPPFLAGS= -DHAVE_CONFIG_H -DOM_NDEBUG
libomalloc_g_la_CPPFLAGS= -DHAVE_CONFIG_H

BUILT_SOURCES = omTables.inc omTables.h

omTables.inc: omTables omalloc.h
	./omTables > omTables.xx && mv omTables.xx  $@

omTables.h: omTables
	./omTables 1 >omTables.yy && mv omTables.yy $@

noinst_PROGRAMS = omTables
omTables_SOURCES = omAllocPrivate.h omConfig.h omTables.c mylimits.h
omTables_CPPFLAGS = -DOM_GENERATE_INC  -DHAVE_CONFIG_H

CLEANFILES = $(BUILT_SOURCES)

####################################################
## Test program
##
# AM_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc


TESTS = omtTest-g omtTest-m omtTest-r
check_PROGRAMS = $(TESTS)

OMTTESTSSOURCES = omtTestReal.c omtTestDebug.c omtTestKeep.c omtTestError.c omtTest.c

# omtTest_g: libomalloc_g.la

omtTest_g_SOURCES = $(OMTTESTSSOURCES)
omtTest_g_CPPFLAGS = -DHAVE_CONFIG_H
omtTest_g_CFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long
omtTest_g_LDADD = -lomalloc_g
omtTest_g_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc


# omtTest_m: libomalloc_g.la

omtTest_m_CPPFLAGS = -DOM_TEST_MALLOC  -DHAVE_CONFIG_H
omtTest_m_SOURCES  = $(OMTTESTSSOURCES)
omtTest_m_LDADD = -lomalloc_g
omtTest_m_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc

# omtTest_r: libomalloc.la

omtTest_r_SOURCES = $(OMTTESTSSOURCES)
omtTest_r_CPPFLAGS = -DOM_NDEBUG
omtTest_r_CFLAGS = -O3
omtTest_r_LDADD = -lomalloc
omtTest_r_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc

