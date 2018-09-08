ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${GMP_CPPFLAGS}

noinst_LTLIBRARIES=libfglm.la
libfglm_la_SOURCES=fglmzero.cc fglmvec.cc fglmgauss.cc fglmhom.cc fglmcomb.cc

libfglm_la_includedir=$(includedir)/singular/kernel/fglm
libfglm_la_include_HEADERS=fglm.h fglmgauss.h fglmvec.h



TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test
check_PROGRAMS = $(TESTS)
test_SOURCES = test.cc
test_LDADD   = libfglm.la ${builddir}/../../Singular/libSingular.la

CLEANFILES = $(TESTS)
