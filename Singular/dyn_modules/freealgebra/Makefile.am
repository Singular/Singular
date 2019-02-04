ACLOCAL_AMFLAGS = -I ../../m4

if SI_BUILTIN_FREEALGEBRA
  noinst_LTLIBRARIES=freealgebra.la
  P_PROCS_MODULE_LDFLAGS = -module
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
else
  module_LTLIBRARIES=freealgebra.la
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
endif
MYINCLUDES =  -I${top_srcdir} -I${top_builddir} \
      -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
      $(FACTORY_INCLUDES) $(RESOURCES_INCLUDES) $(OMALLOC_INCLUDES) \
      $(FLINT_CFLAGS) $(NTL_CPPFLAGS) $(GMP_CPPFLAGS) $(GOOGLE_PERFTOOLS_CFLAGS)


freealgebra_la_CPPFLAGS  = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
freealgebra_la_LDFLAGS   = ${AM_LDFLAGS} ${P_PROCS_MODULE_LDFLAGS} $(SINGULAR_LDFLAGS)
SOURCES = freealgebra.cc
freealgebra_la_SOURCES   = $(SOURCES)

