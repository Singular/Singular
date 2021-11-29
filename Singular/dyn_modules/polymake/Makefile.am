ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS =  -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
$(PM_INC) $(PM_CFLAGS) \
${FACTORY_INCLUDES} ${RESOURCES_INCLUDES} ${OMALLOC_INCLUDES} \
${FLINT_CFLAGS} $(NTL_CXXFLAGS) ${NTL_CPPFLAGS} ${GMP_CPPFLAGS}
# $(FEXCEPTIONSFRTTI_CXXFLAGS) ??

if SI_BUILTIN_POLYMAKE
  noinst_LTLIBRARIES=polymake.la
##  moduledir = $(libdir)/singular
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS  = -module
else
  module_LTLIBRARIES=polymake.la
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
  P_PROCS_MODULE_LDFLAGS =  -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
endif

polymake_la_LIBADD =
if SING_WIN
if !SI_BUILTIN_POLYMAKE
 polymake_la_LIBADD += ${top_builddir}/Singular/libSingular.la
endif
endif

if SING_HAVE_POLYMAKE
# forcefully enable exceptions for polymake
 CXXFLAGS+= $(FEXCEPTIONSFRTTI_CXXFLAGS)
 PM_CXXFLAGS= ${FEXCEPTIONSFRTTI_CXXFLAGS}
 P_PROCS_MODULE_LDFLAGS+= ${PM_LDFLAGS}
 polymake_la_LIBADD+= ${PM_LIBS}
endif

SOURCES = polymake_conversion.h polymake_conversion.cc \
          polymake_documentation.h polymake_documentation.cc \
	  polymake_wrapper.cc

polymake_la_SOURCES = $(SOURCES)

polymake_la_CPPFLAGS = ${AM_CPPFLAGS} ${P_PROCS_CPPFLAGS_COMMON} ${PM_CXXFLAGS}
polymake_la_CXXFLAGS = ${PM_CXXFLAGS}
polymake_la_CFLAGS   = ${PM_CXXFLAGS}

polymake_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS} $(SINGULAR_LDFLAGS)
