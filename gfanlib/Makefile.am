ACLOCAL_AMFLAGS = -I ../m4

if HAVE_GFANLIB
  libgfan_LTLIBRARIES=libgfan.la
endif

libgfandir = $(libdir)

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -DGMPRATIONAL

SOURCES  = gfanlib_zcone.cpp gfanlib_symmetry.cpp gfanlib_symmetriccomplex.cpp gfanlib_polyhedralfan.cpp gfanlib_zfan.cpp gfanlib_polymakefile.cpp
libgfan_la_SOURCES   = $(SOURCES)

libgfan_includedir  =$(includedir)/gfanlib

libgfan_include_HEADERS = gfanlib_z.h gfanlib_q.h gfanlib_vector.h gfanlib_matrix.h gfanlib_zcone.h gfanlib.h gfanlib_polyhedralfan.h gfanlib_polymakefile.h gfanlib_symmetriccomplex.h gfanlib_symmetry.h gfanlib_zfan.h

DISTCLEANFILES =  config.h

