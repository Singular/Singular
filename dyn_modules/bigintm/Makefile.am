moduledir = $(bindir)/MOD

if ENABLE_P_PROCS_DYNAMIC
  module_LTLIBRARIES=bigintm.la bigintm_g.la 
endif

if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/factory
else
  USE_FACTORY =
endif

CXXTEMPLFLAGS	= ${PIPE} 
## -fno-implicit-templates
## --no-exceptions

DEBUGCXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${CXXTEMPLFLAGS}

NODEBUGCFLAGS   = -O3 -w -fomit-frame-pointer ${PIPE} 
NODEBUGCXXFLAGS = -O3 -w -fomit-frame-pointer ${CXXTEMPLFLAGS} 
##  --no-rtti ?
NODEBUGDEFS = -DNDEBUG -DOM_NDEBUG 

# -DHAVE_CONFIG_H??

MYINCLUDES =  -I${top_srcdir} -I${top_builddir} -I${srcdir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
-I${top_srcdir}/factory/include -I${top_builddir}/factory/include \
$(FACTORY_CFLAGS) $(NTL_CFLAGS) $(GMP_CFLAGS) 


AMLDFLAGS	= 
# -L${top_builddir}/Singular -L${top_builddir}/numeric -L${top_builddir}/kernel \
# -L${top_builddir}/libpolys/polys $(USE_FACTORY) -L${top_builddir}/omalloc -L${top_builddir}/findexec


SOURCES = mod_main.cc bigintm.cc bigintm.h
bigintm_la_SOURCES   = $(SOURCES)
bigintm_g_la_SOURCES = $(SOURCES)


P_PROCS_CPPFLAGS_COMMON = ${PIPE} -DDYNAMIC_VERSION 

bigintm_la_CPPFLAGS   = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGDEFS}
bigintm_g_la_CPPFLAGS = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}

bigintm_la_CXXFLAGS   = ${NODEBUGCXXFLAGS}
bigintm_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}

if ENABLE_P_PROCS_DYNAMIC
  P_PROCS_MODULE_LDFLAGS = ${AMLDFLAGS} -shared -module
  P_PROCS_MODULE_LDFLAGSG = ${AMLDFLAGS} -shared -module
else
  P_PROCS_MODULE_LDFLAGS = ${AMLDFLAGS}
  P_PROCS_MODULE_LDFLAGSG = ${AMLDFLAGS}
endif

# Add under Mac OS X: -twolevel_namespace -weak_reference_mismatches weak -undefined dynamic_lookup

bigintm_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}
bigintm_g_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGSG}




AM_COLOR_TESTS=always

TESTS_ENVIRONMENT  = SINGULARPATH='${abs_top_builddir}/Singular/LIB:${abs_top_srcdir}/Singular/LIB:${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables:${abs_builddir}/.libs:${abs_srcdir}'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}' 
TESTS_ENVIRONMENT += SINGULAR_BIN_DIR='${abs_top_builddir}/Singular' $(SHELL) -x

if WANT_DEBUG
TESTS=test_release.sh test_debug.sh
else
TESTS=test_release.sh
endif

EXTRA_DIST = bigintm.tst test.sh $(TESTS)
