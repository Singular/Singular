ACLOCAL_AMFLAGS = -I ../../m4

if ENABLE_P_PROCS_DYNAMIC
  moduledir = $(libexecdir)/singular/MOD
else
  moduledir = $(libdir)/singular
endif


module_LTLIBRARIES=syzextra.la 

#if ENABLE_P_PROCS_DYNAMIC
# if WANT_DEBUG
# module_LTLIBRARIES+=syzextra_g.la
# endif
#endif

if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/factory
else
  USE_FACTORY =
endif

# CXXTEMPLFLAGS	= ${PIPE} 
# ## -fno-implicit-templates
# 
# DEBUGCXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${CXXTEMPLFLAGS}
# 
# NODEBUGCFLAGS   = -O3 -w -fomit-frame-pointer ${PIPE} 
# NODEBUGCXXFLAGS = -O3 -w -fomit-frame-pointer ${CXXTEMPLFLAGS} -fno-rtti -fno-exceptions
# 
# NODEBUGDEFS = -DNDEBUG -DOM_NDEBUG 

# -DHAVE_CONFIG_H??

MYINCLUDES =  -I${top_srcdir} -I${top_builddir} -I${srcdir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
-I${top_srcdir}/factory/include -I${top_builddir}/factory/include \
$(FACTORY_CFLAGS) $(NTL_CFLAGS) $(GMP_CFLAGS) 


AM_LDFLAGS = ${GOOGLE_PERFTOOL_LDFLAGS} 
# -release ${PACKAGE_VERSION} 

# -L${top_builddir}/Singular -L${top_builddir}/numeric -L${top_builddir}/kernel \
# -L${top_builddir}/libpolys/polys $(USE_FACTORY) -L${top_builddir}/omalloc -L${top_builddir}/resources


SOURCES = mod_main.cc DebugPrint.cc DebugPrint.h myNF.cc myNF.h singularxx_defs.h
syzextra_la_SOURCES   = $(SOURCES)
# syzextra_g_la_SOURCES = $(SOURCES)


P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION 

syzextra_la_CPPFLAGS   = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}
# ${NODEBUGDEFS}
# syzextra_g_la_CPPFLAGS = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}

syzextra_la_CXXFLAGS   = 
# ${NODEBUGCXXFLAGS}
#syzextra_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}

P_PROCS_MODULE_LDFLAGS = ${AM_LDFLAGS} -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup

syzextra_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}

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
# EXTRA_DIST = syzextra.tst ederc.tst test_clear_enum.tst test.sh $(TESTS)
