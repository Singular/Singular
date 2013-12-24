ACLOCAL_AMFLAGS = -I ../m4
# TODO: use ${top_srcdir} instead of .. in the above?

# PIPE = -pipe
# ???
#LD_DYN_FLAGS	= -ldl -rdynamic
#SFLAGS		= -fpic -DPIC
#SLDFLAGS	= -shared
#LD_LIBC	        = -lc
#STATIC_LDFLAGS	= -static


# CXXTEMPLFLAGS =-ftrapv ## -fno-implicit-templates
# 
# if WANT_DEBUG
#   LIB_G=libkernel_g.la
# else
#   LIB_G=
# endif

libkernel_LTLIBRARIES = libkernel.la 
# ${LIB_G}
libkerneldir = $(libdir)/singular

#libkernel_la_CFLAGS   = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE}
#libkernel_g_la_CFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE}
## -pedantic

#libkernel_la_CXXFLAGS   = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
#libkernel_g_la_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)
# -fdiagnostics-show-option

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys $(GMP_CFLAGS) -I${top_srcdir}/factory/include -I${top_builddir}/factory/include $(FACTORY_CFLAGS) $(NTL_CFLAGS)  -DHAVE_CONFIG_H

# libkernel_la_CPPFLAGS   = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
# libkernel_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H

### libkernel_la_LDFLAGS    = -release ${PACKAGE_VERSION}


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
    mod2.h

libkernel_la_SOURCES   = $(SOURCES)
# libkernel_g_la_SOURCES = $(SOURCES)

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
	f5c.h f5data.h f5gb.h f5lists.h

libkernel_includedir  =${includedir}/singular/kernel
libkernel_include_HEADERS   = $(KERNELHEADERS)

if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/factory
else
  USE_FACTORY =
endif

AM_LDFLAGS = -L${top_builddir}/kernel -L${top_builddir}/libpolys/polys -L${top_builddir}/libpolys/coeffs -L${top_builddir}/libpolys/reporter  -L${top_builddir}/libpolys/misc $(USE_FACTORY) -L${top_builddir}/omalloc -L${top_builddir}/resources


### TODO: the following has to be addapted...
TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

# if WANT_DEBUG
# TESTS = test-s-g test-s-r test-d-g test-d-r
# else
TESTS = test-s-r test-d-r
# endif
check_PROGRAMS = $(TESTS)

# test_s_r_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
# test_s_g_CFLAGS = ${PIPE}
# 
# test_s_r_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
# test_s_g_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)

testsources = test.cc
test_s_r_SOURCES = $(testsources)
# test_s_g_SOURCES = $(testsources)

test_s_r_LDADD = -lkernel -lpolys -lcoeffs -lreporter -lmisc $(FACTORY_LIBS) -lomalloc -lresources $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}
# test_s_g_LDADD = -lkernel_g -lpolys_g -lcoeffs_g -lreporter_g -lmisc_g $(FACTORY_LIBS) -lomalloc_g -lresources_g $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS}  ${USEPPROCSDYNAMICLD}

# test_s_r_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
# test_s_g_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H
test_s_r_CPPFLAGS = ${AM_CPPFLAGS}

test_s_r_LDFLAGS = -static ${AM_LDFLAGS}
# test_s_g_LDFLAGS = -static ${AM_LDFLAGS}


# test_d_r_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
# test_d_g_CFLAGS = ${PIPE}
# 
# test_d_r_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
# test_d_g_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)

test_d_r_SOURCES = $(testsources)
# test_d_g_SOURCES = $(testsources)

test_d_r_LDADD = -lkernel -lpolys -lcoeffs  -lreporter -lmisc $(FACTORY_LIBS) -lomalloc -lresources $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}
# test_d_g_LDADD = -lkernel_g -lpolys_g -lcoeffs_g  -lreporter_g -lmisc_g $(FACTORY_LIBS) -lomalloc_g -lresources_g $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS}  ${USEPPROCSDYNAMICLD}

# test_d_r_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
# test_d_g_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H
test_d_r_CPPFLAGS = ${AM_CPPFLAGS}

test_d_r_LDFLAGS = ${AM_LDFLAGS}
# test_d_g_LDFLAGS = ${AM_LDFLAGS}

# These files are built first
# BUILT_SOURCES = MOD

# MOD: ${top_builddir}/libpolys/tests/MOD
# 	ln -snf ${top_builddir}/libpolys/tests/MOD ${builddir}/MOD


CLEANFILES = $(TESTS)
# $(BUILT_SOURCES)
