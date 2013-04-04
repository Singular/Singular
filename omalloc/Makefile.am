ACLOCAL_AMFLAGS = -I ../m4


lib_LTLIBRARIES=libomalloc.la libomalloc_g.la

if WANT_DEBUG
# lib_LTLIBRARIES+=
endif

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

AM_CPPFLAGS =-I${top_srcdir}/.. -I${top_builddir}/..

libomalloc_la_SOURCES=$(SOURCES) $(noinst_HEADERS)
libomalloc_g_la_SOURCES=$(SOURCES) $(noinst_HEADERS)

nodist_libomalloc_la_SOURCES = omTables.inc omTables.h mylimits.h
nodist_libomalloc_g_la_SOURCES = omTables.inc omTables.h mylimits.h

nodist_libomalloc_include_HEADERS = omConfig.h mylimits.h omTables.h

### NOTE: "_g" (debug) means full library
### libomalloc_la_CPPFLAGS= -DOM_ALLOC_INTERNAL -DHAVE_CONFIG_H -DOM_NDEBUG

libomalloc_la_CPPFLAGS  = ${AM_CPPFLAGS} -DOM_ALLOC_INTERNAL -DHAVE_CONFIG_H
libomalloc_g_la_CPPFLAGS= ${AM_CPPFLAGS} -DOM_ALLOC_INTERNAL -DHAVE_CONFIG_H

BUILT_SOURCES = omTables.inc omTables.h

omTables.inc: omTables$(EXEEXT) omalloc.h
	./omTables$(EXEEXT) > omTables.xx && mv omTables.xx  $@

omTables.h: omTables$(EXEEXT)
	./omTables$(EXEEXT) 1 >omTables.yy && mv omTables.yy $@

noinst_PROGRAMS = omTables
omTables_SOURCES = omAllocPrivate.h omTables.c mylimits.h
nodist_omTables_SOURCES = omConfig.h
omTables_CPPFLAGS = ${AM_CPPFLAGS} -DOM_GENERATE_INC  -DHAVE_CONFIG_H

CLEANFILES = $(BUILT_SOURCES)

####################################################
## Test program
##
# AM_LDFLAGS = -L${top_builddir}/omalloc -L${top_builddir}/../omalloc

TESTS = omtTest-s-g omtTest-s-m omtTest-d-g omtTest-d-m

if WANT_DEBUG
# TESTS+=
endif

check_PROGRAMS = $(TESTS)


EXTRA_PROGRAMS = omtTest-s-r omtTest-d-r
#### tests fail: omtTest-s-r omtTest-d-r due to  
# !!!expect omError_UnalignedAddr
# ***omError_UnalignedAddr: addr is unaligned
#  occured at:  ../../omalloc/omtTestError.c:260
#   occured for (invalid) addr: 0x7fc5c3602811 (error=14)
# +++ok  omError_UnalignedAddr
# 
# !!!expect omError_NullSizeAlloc
# 2669 Segmentation fault 

OMTTESTSSOURCES = omtTestReal.c omtTestDebug.c omtTestKeep.c omtTestError.c omtTest.c

omtTest_s_g_SOURCES = $(OMTTESTSSOURCES)
omtTest_s_g_CPPFLAGS= ${AM_CPPFLAGS} -DHAVE_CONFIG_H
omtTest_s_g_CFLAGS  = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE}
omtTest_s_g_LDADD   = libomalloc_g.la
omtTest_s_g_LDFLAGS = -L${builddir} -static

omtTest_s_m_CPPFLAGS= ${AM_CPPFLAGS} -DOM_TEST_MALLOC  -DHAVE_CONFIG_H
omtTest_s_m_SOURCES = $(OMTTESTSSOURCES)
omtTest_s_m_LDADD   = libomalloc_g.la
omtTest_s_m_LDFLAGS = -L${builddir} -static

omtTest_s_r_SOURCES = $(OMTTESTSSOURCES)
omtTest_s_r_CPPFLAGS= ${AM_CPPFLAGS} -DOM_NDEBUG
omtTest_s_r_CFLAGS  = -O3 ${PIPE}
omtTest_s_r_LDADD   = libomalloc.la
omtTest_s_r_LDFLAGS = -L${builddir} -static



omtTest_d_g_SOURCES = $(OMTTESTSSOURCES)
omtTest_d_g_CPPFLAGS= ${AM_CPPFLAGS} -DHAVE_CONFIG_H
omtTest_d_g_CFLAGS  = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE}
omtTest_d_g_LDADD   = libomalloc_g.la
omtTest_d_g_LDFLAGS = -L${builddir}

omtTest_d_m_CPPFLAGS= ${AM_CPPFLAGS} -DOM_TEST_MALLOC  -DHAVE_CONFIG_H
omtTest_d_m_SOURCES = $(OMTTESTSSOURCES)
omtTest_d_m_LDADD   = libomalloc_g.la
omtTest_d_m_LDFLAGS = -L${builddir}

omtTest_d_r_SOURCES = $(OMTTESTSSOURCES)
omtTest_d_r_CPPFLAGS= ${AM_CPPFLAGS} -DOM_NDEBUG
omtTest_d_r_CFLAGS  = -O3 ${PIPE}
omtTest_d_r_LDADD   = libomalloc.la
omtTest_d_r_LDFLAGS = -L${builddir}
