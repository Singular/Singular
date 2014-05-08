ACLOCAL_AMFLAGS = -I ../../m4

SOURCES = polymake_conversion.cc polymake_documentation.cc polymake_wrapper.cc

AM_CPPFLAGS =  -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
$(PM_INC) $(PM_CFLAGS) $(POLYMAKE_CXXFLAGS) \
${FACTORY_INCLUDES} ${RESOURCES_INCLUDES} ${OMALLOC_INCLUDES} \
${FLINT_CFLAGS} ${NTL_CFLAGS} ${GMP_CFLAGS} 

module_LTLIBRARIES=polymake.la

#if SING_HAVE_POLYMAKE
#if ENABLE_P_PROCS_DYNAMIC
##### should be disabled in the source code
#endif
#endif

if ENABLE_P_PROCS_DYNAMIC
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION 
  P_PROCS_MODULE_LDFLAGS =  -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
else
  moduledir = $(libdir)/singular
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS = -module -static
endif

# forcefully enable exceptions for polymake
CXXFLAGS += $(POLYMAKE_CXXFLAGS)

polymake_la_SOURCES = $(SOURCES)

polymake_la_CPPFLAGS = ${AM_CPPFLAGS} ${P_PROCS_CPPFLAGS_COMMON} ${POLYMAKE_CXXFLAGS}
polymake_la_CXXFLAGS = ${POLYMAKE_CXXFLAGS}
polymake_la_CFLAGS   = ${POLYMAKE_CXXFLAGS}

polymake_la_LDFLAGS = ${PM_LDFLAGS} ${P_PROCS_MODULE_LDFLAGS}
polymake_la_LIBADD  = ${PM_LIBS}
