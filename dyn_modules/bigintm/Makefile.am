ACLOCAL_AMFLAGS = -I ../../m4

# moduledir = $(libexecdir)/singular/MOD

if ENABLE_P_PROCS_DYNAMIC
#  noinst_LTLIBRARIES=bigintm.la 
  check_LTLIBRARIES=bigintm.la 
  
# if WANT_DEBUG
# #  noinst_module_LTLIBRARIES+=bigintm_g.la 
#   check_LTLIBRARIES+=bigintm_g.la 
# endif
  
endif

if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/factory
else
  USE_FACTORY =
endif

# CXXTEMPLFLAGS	= ${PIPE} 
# 
# DEBUGCXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${CXXTEMPLFLAGS}
# 
# NODEBUGCFLAGS   = -O3 -w -fomit-frame-pointer ${PIPE}
# NODEBUGCXXFLAGS = -O3 -w -fomit-frame-pointer ${CXXTEMPLFLAGS} -fno-exceptions -fno-rtti
# ## -fno-implicit-templates?
# NODEBUGDEFS = -DSING_NDEBUG -DOM_NDEBUG 

# -DHAVE_CONFIG_H??

MYINCLUDES =  -I${top_srcdir} -I${top_builddir} -I${srcdir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
-I${top_srcdir}/factory/include -I${top_builddir}/factory/include \
$(FACTORY_CFLAGS) $(NTL_CFLAGS) $(GMP_CFLAGS) 


#### AM_LDFLAGS	= -release ${PACKAGE_VERSION}
# -L${top_builddir}/Singular -L${top_builddir}/numeric -L${top_builddir}/kernel \
# -L${top_builddir}/libpolys/polys $(USE_FACTORY) -L${top_builddir}/omalloc -L${top_builddir}/resources


SOURCES = mod_main.cc bigintm.cc bigintm.h
bigintm_la_SOURCES   = $(SOURCES)
# bigintm_g_la_SOURCES = $(SOURCES)

if ENABLE_P_PROCS_DYNAMIC
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION 
else
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
endif

bigintm_la_CPPFLAGS   = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
#  ${NODEBUGDEFS}
# bigintm_g_la_CPPFLAGS = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}

# bigintm_la_CXXFLAGS   = ${NODEBUGCXXFLAGS}
# bigintm_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}

if ENABLE_P_PROCS_DYNAMIC
  P_PROCS_MODULE_LDFLAGS = ${AM_LDFLAGS} -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
else
  P_PROCS_MODULE_LDFLAGS = ${AM_LDFLAGS}
endif

# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup

bigintm_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}
# bigintm_g_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGSG}


# AM_COLOR_TESTS=always
#
# TESTS_ENVIRONMENT  = SINGULARPATH='${abs_top_builddir}/Singular/LIB:${abs_top_srcdir}/Singular/LIB:${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables:${abs_builddir}/.libs:${abs_srcdir}'
# TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}' 
# TESTS_ENVIRONMENT += SINGULAR_BIN_DIR='${abs_top_builddir}/Singular'
#
# TESTS=test_release.sh
#
# # if WANT_DEBUG
# # TESTS+=test_debug.sh
# # endif
#
# EXTRA_DIST = test.sh $(TESTS)

