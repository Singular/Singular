ACLOCAL_AMFLAGS = -I ../../m4

MYINCLUDES =  -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
$(FACTORY_INCLUDES) $(RESOURCES_INCLUDES) $(OMALLOC_INCLUDES) \
$(FLINT_CFLAGS) $(NTL_CXXFLAGS) $(NTL_CPPFLAGS) $(GMP_CPPFLAGS) $(GOOGLE_PERFTOOLS_CFLAGS)

if SI_BUILTIN_SYSTHREADS
  noinst_LTLIBRARIES=systhreads.la
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS = -module
else
  module_LTLIBRARIES=systhreads.la
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
endif

SOURCES = shared.cc lintree.cc bytebuf.cc thread.cc bytebuf.h lintree.h channel.h syncvar.h threadconf.h thread.h singthreads.h
systhreads_la_SOURCES   = $(SOURCES)
systhreads_la_CPPFLAGS  = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
systhreads_la_LDFLAGS   = ${AM_LDFLAGS} ${P_PROCS_MODULE_LDFLAGS} ${GOOGLE_PERFTOOL_LDFLAGS} $(SINGULAR_LDFLAGS)

if SING_WIN
if !SI_BUILTIN_SYSTHREADS
systhreads_la_LIBADD    = ${top_builddir}/Singular/libSingular.la
endif
endif

