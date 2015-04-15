ACLOCAL_AMFLAGS = -I ../m4

lib_LTLIBRARIES=libomalloc.la

libomalloc_includedir=$(includedir)/omalloc

libomalloc_include_HEADERS = \
omalloc.h		omallocClass.h          omDerivedConfig.h	\
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
omalloc_provide.c omAllocFunc.c    omallocClass.cc

EXTRA_DIST = omalloc.c omtTestAlloc.c omtTest.h omMmap.c

AM_CPPFLAGS =-I${top_srcdir}/.. -I${top_builddir}/..

libomalloc_la_SOURCES=$(SOURCES) $(noinst_HEADERS)

libomalloc_la_LDFLAGS    = -release ${PACKAGE_VERSION}
if WANT_DEBUG
libomalloc_la_CFLAGS    = -g
else !WANT_DEBUG
libomalloc_la_CFLAGS    = -O2
endif

nodist_libomalloc_la_SOURCES = omTables.inc omTables.h mylimits.h
nodist_libomalloc_include_HEADERS = omConfig.h mylimits.h omTables.h

libomalloc_la_CPPFLAGS  = ${AM_CPPFLAGS} -DOM_ALLOC_INTERNAL

BUILT_SOURCES = omTables.inc omTables.h

omTables.inc: omTables$(EXEEXT) omalloc.h
	./omTables$(EXEEXT) > omTables.xx && mv omTables.xx  $@

omTables.h: omTables$(EXEEXT)
	./omTables$(EXEEXT) 1 >omTables.yy && mv omTables.yy $@

noinst_PROGRAMS = omTables
omTables_SOURCES = omAllocPrivate.h omTables.c mylimits.h
nodist_omTables_SOURCES = omConfig.h
omTables_CPPFLAGS = ${AM_CPPFLAGS} -DOM_GENERATE_INC

CLEANFILES = $(BUILT_SOURCES)
DISTCLEANFILES = omConfig.h
####################################################
## Test program

TESTS = omtTest-m omtTest-r

check_PROGRAMS = $(TESTS)

# EXTRA_PROGRAMS = omtTest-r

OMTTESTSSOURCES = omtTestReal.c omtTestDebug.c omtTestKeep.c omtTestError.c omtTest.c

omtTest_m_CPPFLAGS = ${AM_CPPFLAGS} -DOM_TEST_MALLOC
omtTest_m_SOURCES  = $(OMTTESTSSOURCES)
omtTest_m_LDADD    = libomalloc.la

omtTest_r_SOURCES  = $(OMTTESTSSOURCES)
omtTest_r_LDADD    = libomalloc.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = omalloc.pc

