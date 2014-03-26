ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${NTL_CFLAGS} $(FLINT_CFLAGS) ${GMP_CFLAGS}

noinst_LTLIBRARIES=liblinear_algebra.la
liblinear_algebra_la_SOURCES=Minor.cc MinorInterface.cc MinorProcessor.cc linearAlgebra.cc eigenval.cc

liblinear_algebra_la_includedir=$(includedir)/kernel/linear_algebra
liblinear_algebra_la_include_HEADERS=Minor.h MinorInterface.h MinorProcessor.h linearAlgebra.h eigenval.h



