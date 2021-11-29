ACLOCAL_AMFLAGS = -I ../../m4

if SI_BUILTIN_SINGMATHIC
  noinst_LTLIBRARIES=singmathic.la
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS  = -module
else
  module_LTLIBRARIES=singmathic.la
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
  # Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
endif

MYINCLUDES =  -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
$(FACTORY_INCLUDES) $(RESOURCES_INCLUDES) $(OMALLOC_INCLUDES) \
$(FLINT_CFLAGS) $(NTL_CXXFLAGS) $(NTL_CPPFLAGS) $(GMP_CPPFLAGS) $(CPPFLAGS)

singmathic_la_SOURCES  = singmathic.cc
singmathic_la_CPPFLAGS = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
singmathic_la_LDFLAGS  = ${P_PROCS_MODULE_LDFLAGS} ${MATHIC_LIBS} $(SINGULAR_LDFLAGS)

if SING_WIN
if !SI_BUILTIN_SINGMATHIC
 singmathic_la_LIBADD = ${top_builddir}/Singular/libSingular.la
endif
endif

