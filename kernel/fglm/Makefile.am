ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${NTL_CFLAGS} $(FLINT_CFLAGS) ${GMP_CFLAGS}

noinst_LTLIBRARIES=libfglm.la
libfglm_la_SOURCES=fglmzero.cc fglmvec.cc fglmgauss.cc fglmhom.cc fglmcomb.cc

libfglm_la_includedir=$(includedir)/singular/fglm
libfglm_la_include_HEADERS=fglm.h fglmgauss.h fglmvec.h



