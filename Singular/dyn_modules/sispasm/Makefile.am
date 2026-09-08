ACLOCAL_AMFLAGS = -I ../../m4

if SI_BUILTIN_SISPASM
  noinst_LTLIBRARIES=sispasm.la
  P_PROCS_MODULE_LDFLAGS = -module
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
else
  module_LTLIBRARIES=sispasm.la
  moduledir = $(libdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version
endif
MYINCLUDES =  -I${top_srcdir} -I${top_builddir} \
      -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
      $(FACTORY_INCLUDES) $(RESOURCES_INCLUDES) $(OMALLOC_INCLUDES) \
      $(FLINT_CFLAGS) $(NTL_CXXFLAGS) $(NTL_CPPFLAGS) $(GMP_CPPFLAGS) $(GOOGLE_PERFTOOLS_CFLAGS)


sispasm_la_CPPFLAGS  = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
sispasm_la_LDFLAGS   = ${AM_LDFLAGS} ${P_PROCS_MODULE_LDFLAGS} $(SINGULAR_LDFLAGS)
sispasm_la_LIBADD    = $(SPASM_LIB)
SOURCES = sispasm.cc
sispasm_la_SOURCES   = $(SOURCES)

if SING_WIN
if !SI_BUILTIN_SISPASM
sispasm_la_LIBADD   += ${top_builddir}/Singular/libSingular.la
endif
endif

AM_COLOR_TESTS=always
TESTS_ENVIRONMENT  = SINGULARPATH='${abs_top_builddir}/Singular/LIB:${abs_top_srcdir}/Singular/LIB:${abs_builddir}/.libs:${abs_srcdir}'
TESTS_ENVIRONMENT += SINGULAR_BIN_DIR='${abs_top_builddir}/Singular'
TESTS=test_sispasm.sh
EXTRA_DIST=$(TESTS)
