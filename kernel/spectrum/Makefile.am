ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${NTL_CFLAGS} $(FLINT_CFLAGS) ${GMP_CFLAGS}

noinst_LTLIBRARIES=libspectrum.la
libspectrum_la_SOURCES=GMPrat.cc multicnt.cc npolygon.cc semic.cc spectrum.cc splist.cc

libspectrum_la_includedir=$(includedir)/kernel/spectrum
libspectrum_la_include_HEADERS=GMPrat.h multicnt.h npolygon.h semic.h spectrum.h splist.h kmatrix.h



