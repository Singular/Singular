ACLOCAL_AMFLAGS = -I ../../m4

if SI_BUILTIN_MACHINELEARNING
  noinst_LTLIBRARIES = machinelearning.la
##  moduledir = $(libdir)/singular
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS  = -module
else !SI_BUILTIN_MACHINELEARNING
  module_LTLIBRARIES = machinelearning.la
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
###  -export-dynamic -shared -module -avoid-version (add to all DMs?)
endif !SI_BUILTIN_MACHINELEARNING

##########################################################################
AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/kernel -I${top_builddir}/kernel -I${top_srcdir}/libpolys -I${top_builddir}/libpolys $(FACTORY_INCLUDES)  $(GMP_CPPFLAGS) $(NTL_CPPFLAGS) $(FLINT_CFLAGS)

machinelearning_la_SOURCES = machinelearning.cc

### if HAVE_PYTHON???
machinelearning_la_CXXFLAGS = $(PYTHON_CSPEC)
machinelearning_la_CPPFLAGS = $(PYTHON_CPPFLAGS)  ${AM_CPPFLAGS} ${P_PROCS_CPPFLAGS_COMMON}

machinelearning_la_LDFLAGS = $(PYTHON_EXTRA_LIBS)  $(PYTHON_LSPEC) ${AM_LDFLAGS} $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_LDFLAGS) ${P_PROCS_MODULE_LDFLAGS} $(SINGULAR_LDFLAGS)
machinelearning_la_LIBADD = $(PYTHON_EXTRA_LIBS) $(PYTHON_LSPEC) $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_LDFLAGS)
if SING_WIN
if !SI_BUILTIN_MACHINELEARNING
  machinelearning_la_LIBADD += ${top_builddir}/Singular/libSingular.la
endif
endif
