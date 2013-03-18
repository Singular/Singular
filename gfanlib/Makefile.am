ACLOCAL_AMFLAGS = -I ../m4

if HAVE_GFANLIB
  libgfan_LTLIBRARIES=libgfan.la
if WANT_DEBUG
  libgfan_LTLIBRARIES+=libgfan_g.la
endif
endif

libgfandir = $(libdir)/

CXXTEMPLFLAGS =-ftrapv
### -fno-implicit-templates

libgfan_la_CFLAGS   = -O3 -fpermissive -fomit-frame-pointer ${PIPE}
libgfan_g_la_CFLAGS = ${PIPE}

libgfan_la_CXXFLAGS   = -O3 -fpermissive -fomit-frame-pointer ${PIPE} ${CXXTEMPLFLAGS} -fno-exceptions -fno-rtti
libgfan_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -DGMPRATIONAL

libgfan_la_CPPFLAGS   = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
libgfan_g_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H

SOURCES  = gfanlib_zcone.cpp gfanlib_symmetry.cpp gfanlib_symmetriccomplex.cpp gfanlib_polyhedralfan.cpp gfanlib_zfan.cpp gfanlib_polymakefile.cpp
libgfan_la_SOURCES   = $(SOURCES)
libgfan_g_la_SOURCES = $(SOURCES)

libgfan_includedir  =$(includedir)/gfanlib/

libgfan_include_HEADERS = gfanlib_z.h gfanlib_q.h gfanlib_vector.h gfanlib_matrix.h gfanlib_zcone.h gfanlib.h gfanlib_polyhedralfan.h gfanlib_polymakefile.h gfanlib_symmetriccomplex.h gfanlib_zfan.h



