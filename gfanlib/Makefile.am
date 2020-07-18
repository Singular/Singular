ACLOCAL_AMFLAGS = -I ../m4

if HAVE_GFANLIB
  libgfan_LTLIBRARIES=libgfan.la
endif

libgfandir = $(libdir)

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -DGMPRATIONAL

# forcefully enable exceptions
CXXFLAGS+= $(FEXCEPTIONSFRTTI_CXXFLAGS)

AM_CXXFLAGS = @CXX11_FLAG@


SOURCES = gfanlib_circuittableint.cpp gfanlib_mixedvolume.cpp gfanlib_paralleltraverser.cpp gfanlib_polyhedralfan.cpp gfanlib_polymakefile.cpp gfanlib_symmetriccomplex.cpp gfanlib_symmetry.cpp gfanlib_traversal.cpp gfanlib_zcone.cpp gfanlib_zfan.cpp
libgfan_la_SOURCES = $(SOURCES)
libgfan_la_LDFLAGS = $(SINGULAR_LDFLAGS) $(CDDGMPLDFLAGS) $(GMP_LIBS) -release ${PACKAGE_VERSION}

libgfan_includedir =$(includedir)/gfanlib
libgfan_include_HEADERS = config.h gfanlib_mixedvolume.h gfanlib_polymakefile.h gfanlib_symmetry.h gfanlib_vector.h gfanlib_z.h _config.h  gfanlib.h gfanlib_paralleltraverser.h gfanlib_q.h  gfanlib_traversal.h gfanlib_zcone.h gfanlib_circuittableint.h gfanlib_matrix.h gfanlib_polyhedralfan.h gfanlib_symmetriccomplex.h gfanlib_tropicalhomotopy.h gfanlib_zfan.h

DISTCLEANFILES =  config.h

moduledir = $(libexecdir)/singular/MOD
