ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
$(NTL_CXXFLAGS) ${NTL_CPPFLAGS} $(FLINT_CFLAGS) ${GMP_CPPFLAGS}

noinst_LTLIBRARIES=libgroebner_walk.la
libgroebner_walk_la_SOURCES=walkProc.cc walkMain.cc walkSupport.cc

libgroebner_walk_la_includedir=$(includedir)/singular/kernel/groebner_walk
libgroebner_walk_la_include_HEADERS=walkProc.h walkMain.h walkSupport.h



TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test
check_PROGRAMS = $(TESTS)
test_SOURCES = test.cc
test_LDADD   = libgroebner_walk.la $(OMALLOC_LIBS) ${builddir}/../../Singular/libSingular.la $(RESOURCES_LIBS)

CLEANFILES = $(TESTS)
