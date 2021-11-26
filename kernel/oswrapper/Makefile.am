ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${NTL_CPPFLAGS} $(FLINT_CFLAGS) ${GMP_CPPFLAGS}

AM_CXXFLAGS=

noinst_LTLIBRARIES=liboswrapper.la
liboswrapper_la_SOURCES=feread.cc fereadl.c timer.cc rlimit.c vspace.cc

liboswrapper_la_includedir=$(includedir)/singular/kernel/oswrapper
liboswrapper_la_include_HEADERS=feread.h timer.h rlimit.h vspace.h



TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test
check_PROGRAMS = $(TESTS)
test_SOURCES = test.cc
test_LDADD   = liboswrapper.la ${builddir}/../../omalloc/libomalloc.la ${builddir}/../../Singular/libSingular.la

CLEANFILES = $(TESTS)
