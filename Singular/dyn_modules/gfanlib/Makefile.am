ACLOCAL_AMFLAGS = -I ../../m4

SOURCES = singularWishlist.cc singularWishlist.h gfanlib_exceptions.h callgfanlib_conversion.cc callgfanlib_conversion.h bbcone.cc bbcone.h bbfan.cc bbfan.h bbpolytope.cc bbpolytope.h gfan.h std_wrapper.cc std_wrapper.h tropicalDebug.h tropicalDebug.cc tropicalVarietyOfPolynomials.h tropicalVarietyOfPolynomials.cc ppinitialReduction.cc ppinitialReduction.h containsMonomial.cc containsMonomial.h adjustWeights.cc adjustWeights.h tropicalStrategy.cc tropicalStrategy.h initial.cc initial.h witness.cc witness.h lift.cc lift.h flip.cc flip.h tropicalCurves.cc tropicalCurves.h groebnerCone.cc groebnerCone.h startingCone.cc startingCone.h tropicalTraversal.cc tropicalTraversal.h tropicalVarietyOfIdeals.cc tropicalVarietyOfIdeals.h tropicalVariety.cc tropicalVariety.h groebnerFan.cc groebnerFan.h groebnerComplex.cc groebnerComplex.h tropical.cc tropical.h gfanlib.cc

MY_CPPFLAGS = -I${srcdir} -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} ${RESOURCES_INCLUDES} ${OMALLOC_INCLUDES} \
${FLINT_CFLAGS} ${NTL_CPPFLAGS} ${GMP_CPPFLAGS} ${PTHREAD_CFLAGS}

if SI_BUILTIN_GFANLIB
if HAVE_GFANLIB
  noinst_LTLIBRARIES=gfanlib.la
endif
##  moduledir = $(libdir)/singular
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS  = -module
else
if HAVE_GFANLIB
  module_LTLIBRARIES=gfanlib.la
endif
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
  P_PROCS_MODULE_LDFLAGS =  -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
endif

gfanlib_la_SOURCES  = $(SOURCES)

gfanlib_la_CPPFLAGS = ${MY_CPPFLAGS} ${P_PROCS_CPPFLAGS_COMMON} ${CDDGMPCPPFLAGS}
gfanlib_la_LDFLAGS  = ${P_PROCS_MODULE_LDFLAGS} $(SINGULAR_LDFLAGS)

# forcefully enable exceptions for polymake
CXXFLAGS+= $(FEXCEPTIONSFRTTI_CXXFLAGS)

gfanlib_la_LIBADD   =
if SING_WIN
if !SI_BUILTIN_GFANLIB
 gfanlib_la_LIBADD   += ${top_builddir}/Singular/libSingular.la
endif
endif

if HAVE_GFANLIB
 gfanlib_la_LIBADD   += ${abs_top_builddir}/gfanlib/libgfan.la ${CDDGMPLDFLAGS}
endif
