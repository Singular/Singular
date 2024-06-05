ACLOCAL_AMFLAGS = -I ../../m4

SOURCES = gitfan.h gitfan.cc

MY_CPPFLAGS = -I${srcdir} -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} ${RESOURCES_INCLUDES} ${OMALLOC_INCLUDES} \
${FLINT_CFLAGS} $(NTL_CXXFLAGS) ${NTL_CPPFLAGS} ${GMP_CPPFLAGS}

if SI_BUILTIN_GITFAN
  noinst_LTLIBRARIES=gitfan.la
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS  = -module
else
  module_LTLIBRARIES=gitfan.la
  moduledir = $(libdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
  P_PROCS_MODULE_LDFLAGS =  -module -export-dynamic -avoid-version
endif

gitfan_la_SOURCES  = $(SOURCES)

gitfan_la_CPPFLAGS = ${MY_CPPFLAGS} ${P_PROCS_CPPFLAGS_COMMON} ${CDDGMPCPPFLAGS}
gitfan_la_LDFLAGS  = ${P_PROCS_MODULE_LDFLAGS}

if SING_WIN
if !SI_BUILTIN_GITFAN
gitfan_la_LIBADD   = ${top_builddir}/Singular/libSingular.la
endif
endif

