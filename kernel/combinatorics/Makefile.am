ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${NTL_CPPFLAGS} $(FLINT_CFLAGS) ${GMP_CPPFLAGS}

noinst_LTLIBRARIES=libcombinatorics.la
libcombinatorics_la_SOURCES=hdegree.cc hilb.cc hutil.cc

libcombinatorics_la_includedir=$(includedir)/singular/kernel/combinatorics
libcombinatorics_la_include_HEADERS=hutil.h stairc.h hilb.h



TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test
check_PROGRAMS = $(TESTS)
test_SOURCES = test.cc
test_LDADD   = libcombinatorics.la ${builddir}/../libkernelCommon.la ${builddir}/../../omalloc/libomalloc.la ${builddir}/../../Singular/libSingular.la

CLEANFILES = $(TESTS)
