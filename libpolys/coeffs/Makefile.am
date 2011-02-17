AM_CXXFLAGS	= -fno-implicit-templates --no-exceptions
INCLUDES	= -I${top_srcdir}/.. -I${top_srcdir}/../.. -I${top_builddir}/.. -I${srcdir}
AM_LDFLAGS	= -L${top_builddir}/../coeffs -L${top_builddir}/../reporter -L${top_builddir}/../resources -L${top_builddir}/../../omalloc
AM_CPPFLAGS	= -DNDEBUG -DOM_NDEBUG

SOURCES = \
	gnumpc.cc gnumpfl.cc longrat.cc longrat0.cc ffields.cc \
	modulop.cc mpr_complex.cc \
	numbers.cc rintegers.cc rmodulo2m.cc rmodulon.cc shortfl.cc

lib_LIBRARIES=libcoeffs.a libcoeffs_g.a

libcoeffs_a_SOURCES = ${SOURCES}
libcoeffs_g_a_SOURCES = ${SOURCES}

libcoeffs_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -fdiagnostics-show-option -Wno-long-long

noinst_HEADERS= \
	gnumpc.h gnumpfl.h longrat.h modulop.h ffields.h \
	mpr_complex.h mpr_global.h \
	rintegers.h rmodulo2m.h rmodulon.h shortfl.h

include_HEADERS = coeffs.h numbers.h

## debug version:

check_PROGRAMS=test test-g

test_SOURCES=test.cc
test_g_SOURCES=test.cc

test_LDADD = libcoeffs.a -lresources -lreporter -lgmp -lomalloc_debug
test_g_LDADD = libcoeffs_g.a -lresources -lreporter -lgmp -lomalloc_debug

test_g_CXXFLAGS = -g -Wextra -Wall -pedantic -fdiagnostics-show-option -Wno-long-long

