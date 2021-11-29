ACLOCAL_AMFLAGS = -I ../../m4

if SI_BUILTIN_STATICDEMO
  noinst_LTLIBRARIES=staticdemo.la
  P_PROCS_MODULE_LDFLAGS = -module
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION

  MYINCLUDES =  -I${top_srcdir} -I${top_builddir} \
      -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
      $(FACTORY_INCLUDES) $(RESOURCES_INCLUDES) $(OMALLOC_INCLUDES) \
      $(FLINT_CFLAGS) $(NTL_CXXFLAGS) $(NTL_CPPFLAGS) $(GMP_CPPFLAGS) $(GOOGLE_PERFTOOLS_CFLAGS)

  staticdemo_la_SOURCES   = staticdemo.cc
  staticdemo_la_CPPFLAGS  = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
  staticdemo_la_LDFLAGS   = ${AM_LDFLAGS} ${P_PROCS_MODULE_LDFLAGS} $(SINGULAR_LDFLAGS)
else
  EXTRA_DIST=staticdemo.cc
endif

