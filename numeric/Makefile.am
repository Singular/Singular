libnumeric_LTLIBRARIES = libnumeric.la libnumeric_g.la
libnumericdir = $(libdir)/singular

CXXTEMPLFLAGS =  --no-exceptions ## -fno-implicit-templates

libnumeric_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libnumeric_g_la_CFLAGS = ${PIPE}

libnumeric_la_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
libnumeric_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

libnumeric_la_CPPFLAGS   = -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
libnumeric_g_la_CPPFLAGS = -DHAVE_CONFIG_H 

INCLUDES = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys ${GMP_CFLAGS} ${FACTORY_CFLAGS} ${NTL_CFLAGS}

SOURCES  = mpr_base.cc mpr_inout.cc mpr_numeric.cc
libnumeric_la_SOURCES   = $(SOURCES)
libnumeric_g_la_SOURCES = $(SOURCES)

libnumeric_includedir  =$(includedir)/singular/numeric/

libnumeric_include_HEADERS = mpr_base.h mpr_inout.h mpr_numeric.h



