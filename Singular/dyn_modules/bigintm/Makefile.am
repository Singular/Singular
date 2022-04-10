ACLOCAL_AMFLAGS = -I ../../m4

if SI_BUILTIN_BIGINTM
  noinst_LTLIBRARIES=bigintm.la
#  check_LTLIBRARIES=bigintm.la
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS  = -module
else
  module_LTLIBRARIES=bigintm.la
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version -flat_namespace
endif

MYINCLUDES =  -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
$(FACTORY_INCLUDES) $(RESOURCES_INCLUDES) $(OMALLOC_INCLUDES) \
$(FLINT_CFLAGS) $(NTL_CXXFLAGS) $(NTL_CPPFLAGS) $(GMP_CPPFLAGS)

bigintm_la_SOURCES  = mod_main.cc bigintm.cc bigintm.h
bigintm_la_CPPFLAGS = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
bigintm_la_LDFLAGS  = ${P_PROCS_MODULE_LDFLAGS} $(SINGULAR_LDFLAGS)
if SING_WIN
if !SI_BUILTIN_BIGINTM
bigintm_la_LIBADD   = ${top_builddir}/Singular/libSingular.la
endif
endif

