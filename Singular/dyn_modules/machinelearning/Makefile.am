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
AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/kernel \
	      -I${top_builddir}/kernel -I${top_srcdir}/libpolys \
	      -I${top_builddir}/libpolys $(FACTORY_INCLUDES) $(GMP_CPPFLAGS) \
	      -DDATA_PATH=\"$(datadir)\" \
	      -DBIN_PATH=\"$(bindir)\" -I/usr/include/python2.7

machinelearning_la_SOURCES = machinelearning.cc mlpredict.c

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

TESTS	 		= test_bufsize test_init_ml
check_PROGRAMS 		= test_bufsize test_init_ml

#test_bufsize_LDFLAGS	= -lpython3.6m
test_bufsize_LDFLAGS	= -lpython2.7
test_bufsize_SOURCES	= tests/test_bufsize.c mlpredict.c
test_bufsize_CFLAGS	= -Wall -Wextra -I/usr/include/python2.7

#test_init_ml_LDFLAGS	= -lpython3.6m
test_init_ml_LDFLAGS	= -lpython2.7
test_init_ml_SOURCES	= tests/test_init_ml.c mlpredict.c
test_init_ml_CFLAGS	= -Wall -Wextra -I/usr/include/python2.7


nobase_dist_data_DATA 	= \
	ml_python/__init__.py \
	ml_python/predictor_runner.py \
	ml_python/common/__init__.py \
	ml_python/common/constants.py \
	ml_python/common/keyword_vector.py \
	ml_python/common/lookuptable.py \
	ml_python/model/__init__.py \
	ml_python/model/predictor.py \
	ml_singular/extract.lib
