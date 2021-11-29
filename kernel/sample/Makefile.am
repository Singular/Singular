ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
$(NTL_CXXFLAGS) ${NTL_CPPFLAGS} $(FLINT_CFLAGS) ${GMP_CPPFLAGS}

noinst_LTLIBRARIES=libsample.la
libsample_la_SOURCES=sample.cc

# libsample_la_includedir=$(includedir)/singular/kernel/sample
# libsample_la_include_HEADERS=sample.h



