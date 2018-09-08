ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${GMP_CPPFLAGS}

noinst_LTLIBRARIES=libspectrum.la
libspectrum_la_SOURCES=GMPrat.cc multicnt.cc npolygon.cc semic.cc spectrum.cc splist.cc

libspectrum_la_includedir=$(includedir)/singular/kernel/spectrum
libspectrum_la_include_HEADERS=GMPrat.h multicnt.h npolygon.h semic.h spectrum.h splist.h kmatrix.h



TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test
check_PROGRAMS = $(TESTS)
test_SOURCES = test.cc
test_LDADD   = libspectrum.la ${builddir}/../../Singular/libSingular.la

CLEANFILES = $(TESTS)
