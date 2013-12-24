ACLOCAL_AMFLAGS = -I ../m4

# if WANT_DEBUG
#   LIB_G=libnumeric_g.la
# else
#   LIB_G=
# endif
# 
libnumeric_LTLIBRARIES = libnumeric.la
# ${LIB_G}
libnumericdir = $(libdir)/singular

# CXXTEMPLFLAGS =-ftrapv  ## -fno-implicit-templates
# 
# libnumeric_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
# libnumeric_g_la_CFLAGS = ${PIPE}
# 
# libnumeric_la_CXXFLAGS   = -O3 -fomit-frame-pointer ${PIPE} ${CXXTEMPLFLAGS}
# libnumeric_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# # -fdiagnostics-show-option 

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys ${GMP_CFLAGS} -I${top_srcdir}/factory/include -I${top_builddir}/factory/include ${FACTORY_CFLAGS} ${NTL_CFLAGS}  -DHAVE_CONFIG_H

# libnumeric_la_CPPFLAGS   = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
# libnumeric_g_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H 
# libnumeric_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H 

SOURCES  = mpr_base.cc mpr_inout.cc mpr_numeric.cc
libnumeric_la_SOURCES   = $(SOURCES)
# libnumeric_g_la_SOURCES = $(SOURCES)

### libnumeric_la_LDFLAGS    = -release ${PACKAGE_VERSION}

libnumeric_includedir  =$(includedir)/singular/numeric

libnumeric_include_HEADERS = mpr_base.h mpr_inout.h mpr_numeric.h



