moduledir = $(bindir)/MOD

if ENABLE_P_PROCS_DYNAMIC
  module_LTLIBRARIES=syzextra.la syzextra_g.la 
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


AM_LDFLAGS	= -L${abs_top_builddir}/Singular -L${abs_top_builddir}/numeric -L${abs_top_builddir}/kernel \
-L${abs_top_builddir}/libpolys/polys $(USE_FACTORY) -L${abs_top_builddir}/omalloc -L${abs_top_builddir}/findexec


SOURCES = mod_main.cc DebugPrint.cc DebugPrint.h myNF.cc myNF.h singularxx_defs.h
syzextra_la_SOURCES   = $(SOURCES)
syzextra_g_la_SOURCES = $(SOURCES)


P_PROCS_CPPFLAGS_COMMON = ${PIPE} -DDYNAMIC_VERSION 

syzextra_la_CPPFLAGS   = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGDEFS}
syzextra_g_la_CPPFLAGS = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}

syzextra_la_CXXFLAGS   = ${NODEBUGCXXFLAGS}
syzextra_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}

P_PROCS_MODULE_LDFLAGS = -shared -module
P_PROCS_MODULE_LDFLAGSG = -shared -module

# Add under Mac OS X: -twolevel_namespace -weak_reference_mismatches weak -undefined dynamic_lookup

syzextra_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}
syzextra_g_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGSG}




AM_COLOR_TESTS=always

TESTS_ENVIRONMENT  = SINGULARPATH='${abs_top_builddir}/Singular/LIB:${abs_top_srcdir}/Singular/LIB:${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables:${abs_builddir}/.libs:${abs_srcdir}'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}' 
TESTS_ENVIRONMENT += SINGULAR_BIN_DIR='${abs_top_builddir}/Singular' $(SHELL) -x

TESTS=test_release.sh test_debug.sh

EXTRA_DIST = syzextra.tst ederc.tst test.sh $(TESTS)
