ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${FLINT_CFLAGS} $(NTL_CXXFLAGS) ${NTL_CPPFLAGS} ${GMP_CPPFLAGS}

noinst_LTLIBRARIES=libGBEngine.la
libGBEngine_la_SOURCES=khstd.cc kstdfac.cc kstd1.cc kstd2.cc kutil.cc nc.cc sca.cc gr_kstd2.cc kspoly.cc\
  kpolys.cc syz.cc syz0.cc syz1.cc syz2.cc syz3.cc syz4.cc units.cc tgb.cc tgbgauss.cc\
  f5data.cc f5lists.cc f5gb.cc ratgring.cc shiftgb.cc ringgb.cc janet.cc kverify.cc kChinese.cc\
  kLiftstd.cc kstdhelper.cc

libGBEngine_la_includedir=$(includedir)/singular/kernel/GBEngine
libGBEngine_la_include_HEADERS=syz.h kstdfac.h kutil.h khstd.h kstd1.h units.h ratgring.h shiftgb.h nc.h\
  kInline.h tgb.h ringgb.h tgbgauss.h tgb_internal.h f5data.h f5gb.h f5lists.h janet.h kverify.h kChinese.h



TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test
check_PROGRAMS = $(TESTS)
test_SOURCES = test.cc
test_LDADD   = libGBEngine.la $(OMALLOC_LIBS) ${builddir}/../combinatorics/libcombinatorics.la ${builddir}/../linear_algebra/liblinear_algebra.la ${builddir}/../libkernelCommon.la  ${builddir}/../../Singular/libSingular.la $(RESOURCES_LIBS)

CLEANFILES = $(TESTS)

