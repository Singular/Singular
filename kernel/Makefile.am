# ACLOCAL_AMFLAGS = -I ${top_srcdir}/m4

# PIPE = -pipe
# ???
#LD_DYN_FLAGS	= -ldl -rdynamic
#SFLAGS		= -fpic -DPIC
#SLDFLAGS	= -shared
#LD_LIBC	        = -lc
#STATIC_LDFLAGS	= -static


CXXTEMPLFLAGS = --no-exceptions ## -fno-implicit-templates

libkernel_LIBRARIES = libkernel.a libkernel_g.a
libkerneldir = $(libdir)/singular

libkernel_a_CFLAGS   = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE}
libkernel_g_a_CFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE}
## -pedantic

libkernel_a_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
libkernel_g_a_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)
# -fdiagnostics-show-option 

libkernel_a_CPPFLAGS   = -DNDEBUG -DOM_NDEBUG
#libkernel_g_a_CPPFLAGS =

INCLUDES = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys $(GMP_CFLAGS) $(FACTORY_CFLAGS) $(NTL_CFLAGS)

SOURCES = polys.cc febase.cc feread.cc \
    hdegree.cc hilb.cc hutil.cc \
    gr_kstd2.cc \
    ideals.cc \
    khstd.cc kstdfac.cc \
    kstd1.cc kstd2.cc kutil.cc \
    misc.cc \
    fast_maps.cc \
    fglmzero.cc fglmvec.cc fglmgauss.cc fglmhom.cc fglmcomb.cc \
    kspoly.cc kpolys.cc \
    semic.cc \
    syz.cc syz0.cc syz1.cc syz2.cc syz3.cc \
    timer.cc \
    GMPrat.cc multicnt.cc npolygon.cc semic.cc spectrum.cc splist.cc \
    walkProc.cc walkMain.cc walkSupport.cc \
    eigenval.cc units.cc \
    fast_mult.cc digitech.cc \
    tgb.cc tgbgauss.cc ringgb.cc f5data.cc f5lists.cc f5gb.cc \
    f5c.cc ratgring.cc shiftgb.cc gfan.cc \
    linearAlgebra.cc nc.cc preimage.cc \
    fegetopt.c mod2.h 

libkernel_a_SOURCES   = $(SOURCES)
libkernel_g_a_SOURCES = $(SOURCES)

KERNELHEADERS = polys.h hutil.h stairc.h ideals.h \
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
	fegetopt.h \
	ratgring.h shiftgb.h nc.h \
	gfan.h preimage.h longrat.h timer.h kInline.h modulop.h fast_mult.h \
	digitech.h tgb.h ringgb.h tgbgauss.h tgb_internal.h \
	linearAlgebra.h page.h

libkernel_includedir  =${includedir}/singular/kernel/
libkernel_include_HEADERS   = $(KERNELHEADERS)

if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/factory
else
  USE_FACTORY =
endif

AM_LDFLAGS = -L${top_builddir}/kernel -L${top_builddir}/libpolys/polys -L${top_builddir}/libpolys/coeffs -L${top_builddir}/libpolys/reporter -L${top_builddir}/libpolys/resources -L${top_builddir}/libpolys/misc $(USE_FACTORY) -L${top_builddir}/omalloc


TESTS_ENVIRONMENT = SINGULARPATH='${top_builddir}/libpolys/tests'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${top_builddir}' 

TESTS = testg testr 
check_PROGRAMS = $(TESTS)

testr_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
testg_CFLAGS = ${PIPE}

testr_CXXFLAGS = -O3 -fomit-frame-pointer --no-rtti -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
testg_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)

testsources = test.cc f5c.h f5data.h f5gb.h f5lists.h
testr_SOURCES = $(testsources)
testg_SOURCES = $(testsources)

testr_LDADD = -lkernel -lpolys -lcoeffs -lresources -lreporter -lmisc $(FACTORY_LIBS) -lomalloc $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}
testg_LDADD = -lkernel_g -lpolys_g -lcoeffs_g -lresources_g -lreporter_g -lmisc_g $(FACTORY_LIBS) -lomalloc_g $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS}  ${USEPPROCSDYNAMICLD}

testr_CPPFLAGS = -DNDEBUG -DOM_NDEBUG
#testg_CPPFLAGS = 

CLEANFILES = $(TESTS)



