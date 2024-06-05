ACLOCAL_AMFLAGS = -I ../../m4

if SI_BUILTIN_PYOBJECT
  noinst_LTLIBRARIES = pyobject.la
##  moduledir = $(libdir)/singular
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS  = -module
else !SI_BUILTIN_PYOBJECT
  module_LTLIBRARIES = pyobject.la
  moduledir = $(libdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version
###  -export-dynamic -shared -module -avoid-version (add to all DMs?)
endif !SI_BUILTIN_PYOBJECT
pyobject_la_SOURCES = pyobject.cc


##########################################################################
AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/kernel -I${top_builddir}/kernel -I${top_srcdir}/libpolys -I${top_builddir}/libpolys $(FACTORY_INCLUDES)  $(GMP_CPPFLAGS) $(NTL_CXXFLAGS) $(NTL_CPPFLAGS) $(FLINT_CFLAGS)


### if HAVE_PYTHON???
pyobject_la_CXXFLAGS = $(PYTHON_CSPEC)
pyobject_la_CPPFLAGS = $(PYTHON_CPPFLAGS)  ${AM_CPPFLAGS} ${P_PROCS_CPPFLAGS_COMMON}

pyobject_la_LDFLAGS = $(PYTHON_EXTRA_LIBS)  $(PYTHON_LSPEC) ${AM_LDFLAGS} $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_LDFLAGS) ${P_PROCS_MODULE_LDFLAGS} $(SINGULAR_LDFLAGS)
pyobject_la_LIBADD = $(PYTHON_EXTRA_LIBS) $(PYTHON_LSPEC) $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_LDFLAGS)

if SING_WIN
if !SI_BUILTIN_PYOBJECT
  pyobject_la_LIBADD += ${top_builddir}/Singular/libSingular.la
endif
endif
