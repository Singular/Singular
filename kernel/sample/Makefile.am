ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
-I${top_srcdir}/factory/include -I${top_builddir}/factory/include ${FACTORY_CFLAGS} \
${GMP_CFLAGS} ${NTL_CFLAGS} $(FLINT_CFLAGS)

noinst_LTLIBRARIES=libsample.la
libsample_la_SOURCES=sample.cc

# libsample_la_includedir=$(includedir)/singular/sample
# libsample_la_include_HEADERS=sample.h



