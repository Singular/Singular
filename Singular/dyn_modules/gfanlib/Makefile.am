ACLOCAL_AMFLAGS = -I ../../m4

SOURCES = bbcone.cc bbcone.h bbfan.cc bbfan.h bbpolytope.cc bbpolytope.h gfan.h gitfan.cc gitfan.h gfanlib.cc

MY_CPPFLAGS =  -I${top_srcdir} -I${top_builddir} \
-I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} ${RESOURCES_INCLUDES} ${OMALLOC_INCLUDES} \
${FLINT_CFLAGS} ${NTL_CFLAGS} ${GMP_CFLAGS} 

if SI_BUILTIN_GFANLIB
  noinst_LTLIBRARIES=gfanlib.la
##  moduledir = $(libdir)/singular
  P_PROCS_CPPFLAGS_COMMON = -DSTATIC_VERSION
  P_PROCS_MODULE_LDFLAGS  = -module
else
  module_LTLIBRARIES=gfanlib.la
  moduledir = $(libexecdir)/singular/MOD
  P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION 
# Add under Mac OS X: -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
  P_PROCS_MODULE_LDFLAGS =  -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
endif

gfanlib_la_SOURCES  = $(SOURCES)

gfanlib_la_CPPFLAGS = ${MY_CPPFLAGS} ${P_PROCS_CPPFLAGS_COMMON} ${CDDGMPCPPFLAGS}
gfanlib_la_LDFLAGS  = ${P_PROCS_MODULE_LDFLAGS}

if HAVE_GFANLIB
 gfanlib_la_LIBADD   = ${abs_top_builddir}/gfanlib/libgfan.la ${CDDGMPLDFLAGS}
endif

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
