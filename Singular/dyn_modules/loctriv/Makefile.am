ACLOCAL_AMFLAGS = -I ../../m4

if SI_BUILTIN_LOCTRIV
  noinst_LTLIBRARIES=loctriv.la
  P_PROCS_MODULE_LDFLAGS = -module
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
else
  module_LTLIBRARIES=loctriv.la
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version -flat_namespace
endif
MYINCLUDES =  -I${top_srcdir} -I${top_builddir} \
      -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
      $(FACTORY_INCLUDES) $(RESOURCES_INCLUDES) $(OMALLOC_INCLUDES) \
      $(FLINT_CFLAGS) $(NTL_CXXFLAGS) $(NTL_CPPFLAGS) $(GMP_CPPFLAGS) $(GOOGLE_PERFTOOLS_CFLAGS)


loctriv_la_CPPFLAGS  = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
loctriv_la_LDFLAGS   = ${AM_LDFLAGS} ${P_PROCS_MODULE_LDFLAGS} $(SINGULAR_LDFLAGS)
SOURCES = loctriv.cc
loctriv_la_SOURCES   = $(SOURCES)

if SING_WIN
if !SI_BUILTIN_LOCTRIV
loctriv_la_LIBADD   = ${top_builddir}/Singular/libSingular.la
endif
endif

