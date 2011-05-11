lib_LIBRARIES=libcoeffs.a libcoeffs_g.a
CXXTEMPLFLAGS      = -fno-implicit-templates --no-exceptions

libcoeffs_a_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libcoeffs_g_a_CFLAGS = ${PIPE}

libcoeffs_a_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
libcoeffs_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

libcoeffs_a_CPPFLAGS   = -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H
libcoeffs_g_a_CPPFLAGS = -DHAVE_CONFIG_H


include_HEADERS = coeffs.h numbers.h

noinst_HEADERS= \
	gnumpc.h gnumpfl.h longrat.h modulop.h ffields.h \
	rintegers.h rmodulo2m.h rmodulon.h shortfl.h \
	mpr_complex.h mpr_global.h

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} ${GMP_CFLAGS} ${NTL_CFLAGS}

SOURCES = \
	numbers.cc rintegers.cc rmodulo2m.cc rmodulon.cc shortfl.cc \
	gnumpc.cc gnumpfl.cc longrat.cc longrat0.cc ffields.cc \
	modulop.cc mpr_complex.cc
libcoeffs_a_SOURCES   = $(SOURCES)
libcoeffs_g_a_SOURCES = $(SOURCES)

libcoeffs_a_includedir  =$(includedir)/libpolys/coeffs
libcoeffs_g_a_includedir=$(includedir)/libpolys/coeffs

COEFFSHEADERS  = coeffs.h numbers.h
libcoeffs_a_include_HEADERS   = $(COEFFSHEADERS)
libcoeffs_g_a_include_HEADERS = $(COEFFSHEADERS)



## for testing...
AM_LDFLAGS	= -L${top_builddir}/coeffs -L${top_builddir}/reporter -L${top_builddir}/resources -L${top_builddir}/misc -L${top_builddir}/../factory -L${top_builddir}/../omalloc

check_PROGRAMS = test test-g

test_SOURCES   = test.cc
test_g_SOURCES = test.cc

test_LDADD   = -lcoeffs ${GMP_LIBS} ${NTL_LIBS} -lresources -lreporter -lmisc -lfactory -lomalloc
test_g_LDADD = -lcoeffs_g ${GMP_LIBS} ${NTL_LIBS} -lresources_g -lreporter_g -lmisc_g -lfactory_g -lomalloc_debug



test_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
test_g_CFLAGS = ${PIPE}

test_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS} -fimplicit-templates
test_g_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS} -fimplicit-templates
# -fdiagnostics-show-option 

test_CPPFLAGS   = -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H
test_g_CPPFLAGS = -DHAVE_CONFIG_H
