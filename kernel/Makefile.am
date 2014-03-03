ACLOCAL_AMFLAGS = -I ../m4
# TODO: use ${top_srcdir} instead of .. in the above?

SUBDIRS=sample numeric
# kernelsample   

noinst_LTLIBRARIES = libkernel.la 
###### libkerneldir = $(libdir)/singular

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys $(GMP_CFLAGS) -I${top_srcdir}/factory/include -I${top_builddir}/factory/include $(FACTORY_CFLAGS) $(NTL_CFLAGS)  

SOURCES = polys.cc febase.cc feread.cc \
    hdegree.cc hilb.cc hutil.cc \
    ideals.cc \
    khstd.cc kstdfac.cc \
    kstd1.cc kstd2.cc kutil.cc \
    nc.cc sca.cc gr_kstd2.cc \
    misc.cc \
    fast_maps.cc \
    fglmzero.cc fglmvec.cc fglmgauss.cc fglmhom.cc fglmcomb.cc \
    kspoly.cc kpolys.cc \
    syz.cc syz0.cc syz1.cc syz2.cc syz3.cc \
    timer.cc \
    GMPrat.cc multicnt.cc npolygon.cc semic.cc spectrum.cc splist.cc \
    walkProc.cc walkMain.cc walkSupport.cc \
    eigenval.cc units.cc \
    fast_mult.cc digitech.cc \
    tgb.cc tgbgauss.cc ringgb.cc f5data.cc f5lists.cc f5gb.cc f5c.cc \
    ratgring.cc shiftgb.cc \
    linearAlgebra.cc preimage.cc \
    mod2.h \
    janet.cc interpolation.cc minpoly.cc \
    Minor.cc MinorInterface.cc MinorProcessor.cc
 

libkernel_la_SOURCES   = $(SOURCES)


KERNELHEADERS = mod2.h polys.h hutil.h stairc.h ideals.h \
	structs.h \
	syz.h \
	fast_maps.h \
	febase.h \
	walkProc.h walkMain.h walkSupport.h\
	kstdfac.h kmatrix.h\
	kutil.h \
	khstd.h kstd1.h \
	fglm.h fglmgauss.h fglmvec.h \
	GMPrat.h multicnt.h npolygon.h semic.h spectrum.h splist.h \
	eigenval.h units.h \
	ratgring.h shiftgb.h nc.h \
	preimage.h timer.h kInline.h fast_mult.h \
	digitech.h tgb.h ringgb.h tgbgauss.h tgb_internal.h \
	linearAlgebra.h \
	f5c.h f5data.h f5gb.h f5lists.h \
	janet.h interpolation.h minpoly.h \
	Minor.h MinorInterface.h MinorProcessor.h Cache.h CacheImplementation.h

libkernel_la_includedir  =${includedir}/singular/kernel
libkernel_la_include_HEADERS   = $(KERNELHEADERS)

if ENABLE_FACTORY
  USE_FACTORY = ${top_builddir}/factory/libfactory.la
else
  USE_FACTORY =
endif

libkernel_la_LIBADD   = ${builddir}/sample/libsample.la ${builddir}/numeric/libnumeric.la \
${top_builddir}/libpolys/polys/libpolys.la $(USE_FACTORY) 
### libkernel_la_LDFLAGS    = -release ${PACKAGE_VERSION}

### TODO: the following has to be addapted...
TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test

check_PROGRAMS = $(TESTS)

test_SOURCES = test.cc
test_LDADD = libkernel.la ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}


# These files are built first
# BUILT_SOURCES = MOD

# MOD: ${top_builddir}/libpolys/tests/MOD
# 	ln -snf ${top_builddir}/libpolys/tests/MOD ${builddir}/MOD


CLEANFILES = $(TESTS)
# $(BUILT_SOURCES)

