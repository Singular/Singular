ACLOCAL_AMFLAGS = -I ../m4

noinst_LTLIBRARIES = libkernelCommon.la libkernel.la

AM_CPPFLAGS = \
-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
$(FACTORY_INCLUDES) $(NTL_CXXFLAGS) ${NTL_CPPFLAGS} ${FLINT_CFLAGS} $(GMP_CPPFLAGS)

SOURCES = polys.cc \
    ideals.cc \
    fast_mult.cc digitech.cc \
    preimage.cc \
    mod2.h

libkernelCommon_la_SOURCES  = $(SOURCES)
libkernelCommon_la_LIBADD   = ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD} \
${abs_top_builddir}/libpolys/polys/libpolys.la
libkernelCommon_la_LDFLAGS = $(SINGULAR_LDFLAGS)

###### libkerneldir = $(libdir)/singular

KERNELHEADERS = mod2.h structs.h polys.h ideals.h \
	preimage.h fast_mult.h digitech.h

libkernel_la_SOURCES=$(KERNELHEADERS)
libkernel_la_LDFLAGS = $(SINGULAR_LDFLAGS)

libkernel_la_includedir=${includedir}/singular/kernel
libkernel_la_include_HEADERS=$(KERNELHEADERS)

SUBDIRS=numeric fglm groebner_walk combinatorics spectrum linear_algebra maps GBEngine oswrapper

libkernel_la_LIBADD   = \
${builddir}/numeric/libnumeric.la \
${builddir}/fglm/libfglm.la \
${builddir}/groebner_walk/libgroebner_walk.la \
${builddir}/combinatorics/libcombinatorics.la \
${builddir}/spectrum/libspectrum.la \
${builddir}/linear_algebra/liblinear_algebra.la \
${builddir}/maps/libmaps.la \
${builddir}/GBEngine/libGBEngine.la \
${builddir}/oswrapper/liboswrapper.la \
${builddir}/libkernelCommon.la

### TODO: the following has to be addapted...
TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test

check_PROGRAMS = $(TESTS)

test_SOURCES = test.cc
test_LDADD   = libkernel.la ${builddir}/../omalloc/libomalloc.la ${builddir}/../Singular/libSingular.la

# These files are built first
# BUILT_SOURCES = MOD

# MOD: ${top_builddir}/libpolys/tests/MOD
# 	ln -snf ${top_builddir}/libpolys/tests/MOD ${builddir}/MOD


CLEANFILES = $(TESTS)
# $(BUILT_SOURCES)


