ACLOCAL_AMFLAGS = -I ../m4
# TODO: use ${top_srcdir} instead of .. in the above?

SUBDIRS=sample numeric fglm groebner_walk combinatorics spectrum linear_algebra maps
# kernelsample   

noinst_LTLIBRARIES = libkernel.la 
###### libkerneldir = $(libdir)/singular

AM_CPPFLAGS = \
-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
$(FACTORY_INCLUDES) $(NTL_CFLAGS) $(FLINT_CFLAGS) $(GMP_CFLAGS)

SOURCES = polys.cc febase.cc feread.cc \
    ideals.cc \
    khstd.cc kstdfac.cc \
    kstd1.cc kstd2.cc kutil.cc \
    nc.cc sca.cc gr_kstd2.cc \
    misc.cc \
    kspoly.cc kpolys.cc \
    syz.cc syz0.cc syz1.cc syz2.cc syz3.cc \
    timer.cc \
    units.cc \
    fast_mult.cc digitech.cc \
    tgb.cc tgbgauss.cc ringgb.cc f5data.cc f5lists.cc f5gb.cc f5c.cc \
    ratgring.cc shiftgb.cc \
    preimage.cc \
    mod2.h \
    janet.cc interpolation.cc minpoly.cc
 
libkernel_la_SOURCES   = $(SOURCES)

KERNELHEADERS = mod2.h polys.h stairc.h ideals.h \
	structs.h \
	syz.h \
	fast_maps.h \
	febase.h \
	kstdfac.h \
	kutil.h \
	khstd.h kstd1.h \
	units.h \
	ratgring.h shiftgb.h nc.h \
	preimage.h timer.h kInline.h fast_mult.h \
	digitech.h tgb.h ringgb.h tgbgauss.h tgb_internal.h \
	f5c.h f5data.h f5gb.h f5lists.h \
	janet.h interpolation.h minpoly.h \
	Cache.h CacheImplementation.h

libkernel_la_includedir  =${includedir}/singular/kernel
libkernel_la_include_HEADERS   = $(KERNELHEADERS)

libkernel_la_LIBADD   = \
${builddir}/sample/libsample.la \
${builddir}/numeric/libnumeric.la \
${builddir}/fglm/libfglm.la \
${builddir}/groebner_walk/libgroebner_walk.la \
${builddir}/combinatorics/libcombinatorics.la \
${builddir}/spectrum/libspectrum.la \
${builddir}/linear_algebra/liblinear_algebra.la \
${builddir}/maps/libmaps.la \
${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD} \
${abs_top_builddir}/libpolys/polys/libpolys.la

### TODO: the following has to be addapted...
TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test

check_PROGRAMS = $(TESTS)

test_SOURCES = test.cc
test_LDADD   = libkernel.la

# These files are built first
# BUILT_SOURCES = MOD

# MOD: ${top_builddir}/libpolys/tests/MOD
# 	ln -snf ${top_builddir}/libpolys/tests/MOD ${builddir}/MOD


CLEANFILES = $(TESTS)
# $(BUILT_SOURCES)

