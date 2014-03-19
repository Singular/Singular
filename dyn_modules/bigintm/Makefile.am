ACLOCAL_AMFLAGS = -I ../../m4

# moduledir = $(libexecdir)/singular/MOD

if ENABLE_P_PROCS_DYNAMIC
  noinst_LTLIBRARIES=bigintm.la 
#  check_LTLIBRARIES=bigintm.la 
  
endif

MYINCLUDES =  -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
$(FACTORY_INCLUDES) $(RESOURCES_INCLUDES) $(OMALLOC_INCLUDES) \
$(FLINT_CFLAGS) $(NTL_CFLAGS) $(GMP_CFLAGS) 

bigintm_la_SOURCES   = mod_main.cc bigintm.cc bigintm.h

if ENABLE_P_PROCS_DYNAMIC
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION 
else
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
endif

bigintm_la_CPPFLAGS   = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}

if ENABLE_P_PROCS_DYNAMIC
  P_PROCS_MODULE_LDFLAGS = -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
  # Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
else
  P_PROCS_MODULE_LDFLAGS = 
endif

bigintm_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}

# AM_COLOR_TESTS=always
#
# TESTS_ENVIRONMENT  = SINGULARPATH='${abs_top_builddir}/Singular/LIB:${abs_top_srcdir}/Singular/LIB:${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables:${abs_builddir}/.libs:${abs_srcdir}'
# TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'
# TESTS_ENVIRONMENT += SINGULAR_BIN_DIR='${abs_top_builddir}/Singular'
#
# TESTS=test_release.sh
#
# EXTRA_DIST = test.sh $(TESTS)

