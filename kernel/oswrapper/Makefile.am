ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${NTL_CFLAGS} $(FLINT_CFLAGS) ${GMP_CFLAGS}

noinst_LTLIBRARIES=liboswrapper.la
liboswrapper_la_SOURCES=feread.cc fereadl.c timer.cc rlimit.c

liboswrapper_la_includedir=$(includedir)/singular/owrapper
liboswrapper_la_include_HEADERS=feread.h timer.h rlimit.h



