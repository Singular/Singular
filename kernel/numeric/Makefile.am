ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
-I${top_srcdir}/factory/include -I${top_builddir}/factory/include ${FACTORY_CFLAGS} \
${GMP_CFLAGS} ${NTL_CFLAGS} $(FLINT_CFLAGS)

noinst_LTLIBRARIES=libnumeric.la
libnumeric_la_SOURCES=mpr_base.cc mpr_inout.cc mpr_numeric.cc

libnumeric_la_includedir=$(includedir)/singular/numeric
libnumeric_la_include_HEADERS=mpr_base.h mpr_inout.h mpr_numeric.h



