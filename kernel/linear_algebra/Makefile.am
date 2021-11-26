ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${FLINT_CFLAGS} ${NTL_CPPFLAGS} ${GMP_CPPFLAGS}

noinst_LTLIBRARIES=liblinear_algebra.la
liblinear_algebra_la_SOURCES= \
Minor.cc MinorInterface.cc MinorProcessor.cc \
linearAlgebra.cc eigenval.cc interpolation.cc minpoly.cc


liblinear_algebra_la_includedir=$(includedir)/singular/kernel/linear_algebra
liblinear_algebra_la_include_HEADERS= \
  Minor.h MinorInterface.h MinorProcessor.h \
  linearAlgebra.h eigenval.h interpolation.h minpoly.h \
  Cache.h CacheImplementation.h



TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test
check_PROGRAMS = $(TESTS)
test_SOURCES = test.cc
test_LDADD   = liblinear_algebra.la ${builddir}/../../omalloc/libomalloc.la ${builddir}/../../Singular/libSingular.la

CLEANFILES = $(TESTS)
