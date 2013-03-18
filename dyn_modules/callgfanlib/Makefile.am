moduledir = $(libexecdir)/singular/MOD

if HAVE_GFANLIB
if ENABLE_P_PROCS_DYNAMIC
  module_LTLIBRARIES=gfanlib.la

if WANT_DEBUG
  module_LTLIBRARIES+=gfanlib_g.la
endif
  
endif
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

#AMLDFLAGS	=  -L${abs_top_builddir}/gfanlib/
# -L${top_builddir}/Singular -L${top_builddir}/numeric -L${top_builddir}/kernel \
# -L${top_builddir}/libpolys/polys $(USE_FACTORY) -L${top_builddir}/omalloc -L${top_builddir}/findexec


SOURCES = bbcone.cc bbcone.h bbfan.cc bbfan.h bbpolytope.cc bbpolytope.h gitfan.cc gitfan.h gfanlib.cc
gfanlib_la_SOURCES   = $(SOURCES)
gfanlib_g_la_SOURCES = $(SOURCES)


P_PROCS_CPPFLAGS_COMMON = ${PIPE} -DDYNAMIC_VERSION

gfanlib_la_CPPFLAGS   = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGDEFS}
gfanlib_g_la_CPPFLAGS = ${MYINCLUDES} ${P_PROCS_CPPFLAGS_COMMON}

gfanlib_la_CXXFLAGS   = ${NODEBUGCXXFLAGS}
gfanlib_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}

gfanlib_la_LIBADD = ${abs_top_builddir}/gfanlib/libgfan.la
gfanlib_g_la_LIBADD = ${gfanlib_la_LIBADD}

if ENABLE_P_PROCS_DYNAMIC
  P_PROCS_MODULE_LDFLAGS = ${AMLDFLAGS} -shared -module
  P_PROCS_MODULE_LDFLAGSG = ${AMLDFLAGS} -shared -module
else
  P_PROCS_MODULE_LDFLAGS = ${AMLDFLAGS}
  P_PROCS_MODULE_LDFLAGSG = ${AMLDFLAGS}
endif

# Add under Mac OS X: -twolevel_namespace -weak_reference_mismatches weak -undefined dynamic_lookup

gfanlib_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}
gfanlib_g_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGSG}




#AM_COLOR_TESTS=always
#
#TESTS_ENVIRONMENT  = SINGULARPATH='${abs_top_builddir}/Singular/LIB:${abs_top_srcdir}/Singular/LIB:${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables:${abs_builddir}/.libs:${abs_srcdir}'
#TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'
#TESTS_ENVIRONMENT += SINGULAR_BIN_DIR='${abs_top_builddir}/Singular' $(SHELL) -x
#
#if WANT_DEBUG
# TESTS=test_release.sh test_debug.sh
#else
# TESTS=test_release.sh
#endif
#
# EXTRA_DIST = gfanlib.tst test.sh $(TESTS)
