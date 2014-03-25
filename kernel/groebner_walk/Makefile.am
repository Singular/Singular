ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${NTL_CFLAGS} $(FLINT_CFLAGS) ${GMP_CFLAGS}

noinst_LTLIBRARIES=libgroebner_walk.la
libgroebner_walk_la_SOURCES=walkProc.cc walkMain.cc walkSupport.cc

libgroebner_walk_la_includedir=$(includedir)/kernel/groebner_walk
libgroebner_walk_la_include_HEADERS=walkProc.h walkMain.h walkSupport.h



