ACLOCAL_AMFLAGS = -I ../../m4

PYTHON_VERSION=2.7

SOURCES =  python_module.cc poly_wrap.cc vector_wrap.cc CF_wrap.cc\
  number_wrap.cc ring_wrap.cc ideal_wrap.cc intvec_wrap.cc \
  interpreter_support.cc \
  CF_wrap.h  Number.h ideal_wrap.h number_wrap.h  ring_wrap.h \
  IIntvec.h  Poly.h   interpreter_support.h  vector_wrap.h \
  Ideal.h    PowerSeries.h  intvec_wrap.h poly_wrap.h wrapper.h

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} ${RESOURCES_INCLUDES} ${OMALLOC_INCLUDES} \
${FLINT_CFLAGS} $(NTL_CXXFLAGS) ${NTL_CPPFLAGS} ${GMP_CPPFLAGS} ${PTHREAD_CFLAGS}

if SI_BUILTIN_PYTHON_MODULE
  noinst_LTLIBRARIES=python_module.la
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS  = -module
else
  module_LTLIBRARIES=python_module.la
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
  P_PROCS_MODULE_LDFLAGS =  -module -export-dynamic -avoid-version -flat_namespace
endif

python_module_la_SOURCES  = $(SOURCES)

python_module_la_CPPFLAGS = ${PYTHON_CSPEC} ${AM_CPPFLAGS} ${P_PROCS_CPPFLAGS_COMMON}
python_module_la_LDFLAGS  = ${P_PROCS_MODULE_LDFLAGS} $(SINGULAR_LDFLAGS)

# forcefully enable exceptions for python
CXXFLAGS = $(FEXCEPTIONSFRTTI_CXXFLAGS)

python_module_la_LIBADD   =
if SING_WIN
if !SI_BUILTIN_PYTHON_MODULE
 python_module_la_LIBADD   += ${top_builddir}/Singular/libSingular.la
endif
endif

if PYTHON_USE
python_module_la_LIBADD   += -lboost_python-${PYTHON_VERSION} ${PYTHON_LSPEC}
endif

python_module_la_CXXFLAGS = ${AM_CXXFLAGS}


python_moduledir =  ${datadir}/singular/LIB
python_module_DATA = cart.py  interpreter.py  perf.py  symm.py  util.py
EXTRA_DIST=cart.py  interpreter.py  perf.py  symm.py  util.py
