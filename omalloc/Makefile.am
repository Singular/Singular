ACLOCAL_AMFLAGS = -I ../m4

lib_LTLIBRARIES=libomalloc.la libomalloc_g.la

libomalloc_includedir=$(includedir)/omalloc

libomalloc_include_HEADERS = \
omalloc.h		omDerivedConfig.h	\
omError.h		omStructs.h		omAllocDecl.h		\
omInlineDecl.h		omBin.h			omMemOps.h		\
omList.h		omGetBackTrace.h	\
omRet2Info.h		omStats.h		omOpts.h		\
omBinPage.h		omAllocSystem.h		\
omAllocPrivate.h	omDebug.h		omInline.h		\
omAllocFunc.h 		mylimits.h

noinst_HEADERS= omPage.h omDefaultConfig.h omReturn.h omGetPageSize.h \
  omMalloc.h omMallocSystem.h

SOURCES=\
omBinPage.c       omList.c         omAllocEmulate.c omDebug.c        \
om_Alloc.c        omDebugCheck.c   omOpts.c         omGetBackTrace.c \
omAllocSystem.c   omError.c        omStats.c	    omRet2Info.c     \
omBin.c           omDebugTrack.c   		     \
omalloc_provide.c omAllocFunc.c

EXTRA_DIST = omalloc.c omtTestAlloc.c omtTest.h omMmap.c

INCLUDES=-I${abs_top_srcdir}/.. -I${abs_top_builddir}/..

libomalloc_la_SOURCES=$(SOURCES) $(noinst_HEADERS)
libomalloc_g_la_SOURCES=$(SOURCES) $(noinst_HEADERS)

nodist_libomalloc_la_SOURCES = omConfig.h mylimits.h omalloc.h omTables.inc omTables.h
nodist_libomalloc_g_la_SOURCES = omConfig.h omalloc.h omTables.inc omTables.h

libomalloc_la_CPPFLAGS= -DHAVE_CONFIG_H -DOM_NDEBUG
libomalloc_g_la_CPPFLAGS= -DHAVE_CONFIG_H

BUILT_SOURCES = omTables.inc omTables.h

omTables.inc: omTables omalloc.h
	./omTables > omTables.xx && mv omTables.xx  $@

omTables.h: omTables
	./omTables 1 >omTables.yy && mv omTables.yy $@

noinst_PROGRAMS = omTables
omTables_SOURCES = omAllocPrivate.h omTables.c mylimits.h
nodist_omTables_SOURCES = omConfig.h
omTables_CPPFLAGS = -DOM_GENERATE_INC  -DHAVE_CONFIG_H

CLEANFILES = $(BUILT_SOURCES)

####################################################
## Test program
##
# AM_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc


TESTS = omtTest-s-g omtTest-s-m omtTest-s-r omtTest-d-g omtTest-d-m omtTest-d-r
check_PROGRAMS = $(TESTS)

OMTTESTSSOURCES = omtTestReal.c omtTestDebug.c omtTestKeep.c omtTestError.c omtTest.c

omtTest_s_g_SOURCES = $(OMTTESTSSOURCES)
omtTest_s_g_CPPFLAGS = -DHAVE_CONFIG_H
omtTest_s_g_CFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE}
omtTest_s_g_LDADD = libomalloc_g.la
omtTest_s_g_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc -static

omtTest_s_m_CPPFLAGS = -DOM_TEST_MALLOC  -DHAVE_CONFIG_H
omtTest_s_m_SOURCES  = $(OMTTESTSSOURCES)
omtTest_s_m_LDADD = libomalloc_g.la
omtTest_s_m_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc -static

omtTest_s_r_SOURCES = $(OMTTESTSSOURCES)
omtTest_s_r_CPPFLAGS = -DOM_NDEBUG
omtTest_s_r_CFLAGS = -O3 ${PIPE}
omtTest_s_r_LDADD = libomalloc.la
omtTest_s_r_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc -static



omtTest_d_g_SOURCES = $(OMTTESTSSOURCES)
omtTest_d_g_CPPFLAGS = -DHAVE_CONFIG_H
omtTest_d_g_CFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE}
omtTest_d_g_LDADD = libomalloc_g.la
omtTest_d_g_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc

omtTest_d_m_CPPFLAGS = -DOM_TEST_MALLOC  -DHAVE_CONFIG_H
omtTest_d_m_SOURCES  = $(OMTTESTSSOURCES)
omtTest_d_m_LDADD = libomalloc_g.la
omtTest_d_m_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc

omtTest_d_r_SOURCES = $(OMTTESTSSOURCES)
omtTest_d_r_CPPFLAGS = -DOM_NDEBUG
omtTest_d_r_CFLAGS = -O3 ${PIPE}
omtTest_d_r_LDADD = libomalloc.la
omtTest_d_r_LDFLAGS = -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/../omalloc
