ACLOCAL_AMFLAGS = -I ../../m4

MYINCLUDES =  -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
$(FACTORY_INCLUDES) $(RESOURCES_INCLUDES) $(OMALLOC_INCLUDES) \
$(FLINT_CFLAGS) $(NTL_CXXFLAGS) $(NTL_CPPFLAGS) $(GMP_CPPFLAGS) $(GOOGLE_PERFTOOLS_CFLAGS)

if SI_BUILTIN_INTERVAL
  noinst_LTLIBRARIES=interval.la
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS = -module
else
  module_LTLIBRARIES=interval.la
  moduledir = $(libdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version
endif

SOURCES = interval.cc interval.h
interval_la_SOURCES   = $(SOURCES)
interval_la_CPPFLAGS  = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
interval_la_LDFLAGS   = ${AM_LDFLAGS} ${P_PROCS_MODULE_LDFLAGS} ${GOOGLE_PERFTOOL_LDFLAGS} $(SINGULAR_LDFLAGS)

if SING_WIN
if !SI_BUILTIN_INTERVAL
interval_la_LIBADD    = ${top_builddir}/Singular/libSingular.la
endif
endif

