ACLOCAL_AMFLAGS = -I ../../m4

SOURCES = nforder.cpp nforder.h \
	  singular.cc \
	  nforder_elt.cc nforder_elt.h \
	  nforder_ideal.cc nforder_ideal.h

MYINCLUDES =  -I${top_srcdir} -I${top_builddir} -I${srcdir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
$(FACTORY_INCLUDES) \
$(FACTORY_CFLAGS) $(NTL_CXXFLAGS) $(NTL_CPPFLAGS) $(GMP_CPPFLAGS) $(GOOGLE_PERFTOOLS_CFLAGS)

if SI_BUILTIN_ORDER
  noinst_LTLIBRARIES=Order.la
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS = -module
else
  module_LTLIBRARIES=Order.la
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION
# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
endif

Order_la_SOURCES   = $(SOURCES)
Order_la_CPPFLAGS  = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
Order_la_LDFLAGS   = ${AM_LDFLAGS} ${P_PROCS_MODULE_LDFLAGS} ${GOOGLE_PERFTOOL_LDFLAGS} $(SINGULAR_LDFLAGS)

if SING_WIN
if !SI_BUILTIN_ORDER
Order_la_LIBADD    = ${top_builddir}/Singular/libSingular.la
endif
endif

# AM_COLOR_TESTS=always
#
# TESTS_ENVIRONMENT  = SINGULARPATH='${abs_top_builddir}/Singular/LIB:${abs_top_srcdir}/Singular/LIB:${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables:${abs_builddir}/.libs:${abs_srcdir}'
# TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'
# TESTS_ENVIRONMENT += SINGULAR_BIN_DIR='${abs_top_builddir}/Singular'
#
# TESTS=test_release.sh
#
# EXTRA_DIST = nforder.tst test.sh $(TESTS)
