ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${GMP_CPPFLAGS}

noinst_LTLIBRARIES=libmaps.la
libmaps_la_SOURCES=fast_maps.cc gen_maps.cc find_perm.cc subst_maps.cc

libmaps_la_includedir=$(includedir)/singular/kernel/maps
libmaps_la_include_HEADERS=fast_maps.h gen_maps.h find_perm.h



TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test
check_PROGRAMS = $(TESTS)
test_SOURCES = test.cc
test_LDADD   = libmaps.la ${builddir}/../../omalloc/libomalloc.la ${builddir}/../../Singular/libSingular.la

CLEANFILES = $(TESTS)
