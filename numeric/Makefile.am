lib_LIBRARIES=libnumeric.a libnumeric_g.a

CXXTEMPLFLAGS =  --no-exceptions ## -fno-implicit-templates

libnumeric_a_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libnumeric_g_a_CFLAGS = ${PIPE}

libnumeric_a_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
libnumeric_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

libnumeric_a_CPPFLAGS   = -DNDEBUG -DOM_NDEBUG
#libnumeric_g_a_CPPFLAGS =

INCLUDES = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys ${GMP_CFLAGS} ${FACTORY_CFLAGS} ${NTL_CFLAGS}

SOURCES  = mpr_base.cc mpr_inout.cc mpr_numeric.cc
libnumeric_a_SOURCES   = $(SOURCES)
libnumeric_g_a_SOURCES = $(SOURCES)

libnumeric_a_includedir  =$(includedir)/numeric/
libnumeric_g_a_includedir=$(includedir)/numeric/

NUMERICHEADERS = mpr_base.h mpr_inout.h mpr_numeric.h

libnumeric_a_include_HEADERS   = $(NUMERICHEADERS)
libnumeric_g_a_include_HEADERS = $(NUMERICHEADERS)



