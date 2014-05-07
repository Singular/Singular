ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/kernel -I${top_builddir}/kernel -I${top_srcdir}/libpolys -I${top_builddir}/libpolys $(FACTORY_INCLUDES)  $(GMP_CFLAGS) $(NTL_CFLAGS) $(FLINT_CFLAGS) -DGIT_VERSION='"$(GIT_VERSION)"'

if ENABLE_P_PROCS_DYNAMIC
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION 
# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
else !ENABLE_P_PROCS_DYNAMIC
  moduledir = $(libdir)/singular
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION 
endif !ENABLE_P_PROCS_DYNAMIC

P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
###  -export-dynamic -shared -module -avoid-version (add to all DMs?)

##########################################################################
module_LTLIBRARIES = pyobject.la

pyobject_la_SOURCES = pyobject.cc

pyobject_la_CXXFLAGS = $(PYTHON_CSPEC)
pyobject_la_CPPFLAGS = $(PYTHON_CPPFLAGS)  ${AM_CPPFLAGS} ${P_PROCS_CPPFLAGS_COMMON}

pyobject_la_LDFLAGS = $(PYTHON_EXTRA_LIBS)  $(PYTHON_LSPEC) ${AM_LDFLAGS} $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_LDFLAGS) ${P_PROCS_MODULE_LDFLAGS}

pyobject_la_LIBADD = $(PYTHON_EXTRA_LIBS) $(PYTHON_LSPEC) $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_LDFLAGS)
