ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} ${RESOURCES_INCLUDES} ${OMALLOC_INCLUDES} \
${NTL_CPPFLAGS} $(FLINT_CFLAGS) ${GMP_CPPFLAGS}

noinst_LTLIBRARIES=libnumeric.la
libnumeric_la_SOURCES=mpr_base.cc mpr_inout.cc mpr_numeric.cc

libnumeric_la_includedir=$(includedir)/singular/kernel/numeric
libnumeric_la_include_HEADERS=mpr_base.h mpr_inout.h mpr_numeric.h



TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test
check_PROGRAMS = $(TESTS)
test_SOURCES = test.cc
test_LDADD   = libnumeric.la ${builddir}/../../omalloc/libomalloc.la ${builddir}/../../Singular/libSingular.la

CLEANFILES = $(TESTS)
