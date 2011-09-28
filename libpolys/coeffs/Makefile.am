libcoeffs_LIBRARIES = libcoeffs.a libcoeffs_g.a
libcoeffsdir = $(libdir)/singular

CXXTEMPLFLAGS =  --no-exceptions 
## -fno-implicit-templates

libcoeffs_a_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libcoeffs_g_a_CFLAGS = ${PIPE}

libcoeffs_a_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
libcoeffs_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

libcoeffs_a_CPPFLAGS   = -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H
libcoeffs_g_a_CPPFLAGS = -DHAVE_CONFIG_H


noinst_HEADERS= \
	gnumpc.h gnumpfl.h longrat.h modulop.h ffields.h \
	rintegers.h rmodulo2m.h rmodulon.h shortfl.h \
	mpr_complex.h mpr_global.h

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} ${FACTORY_CFLAGS} ${GMP_CFLAGS} ${NTL_CFLAGS}

SOURCES = \
	numbers.cc rintegers.cc rmodulo2m.cc rmodulon.cc shortfl.cc \
	gnumpc.cc gnumpfl.cc longrat.cc longrat0.cc ffields.cc \
	modulop.cc mpr_complex.cc

libcoeffs_a_SOURCES   = $(SOURCES)
libcoeffs_g_a_SOURCES = $(SOURCES)

libcoeffs_includedir  =$(includedir)/singular/coeffs
libcoeffs_include_HEADERS = coeffs.h numbers.h si_gmp.h

if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/../factory
else
  USE_FACTORY =
endif

## for testing...
AM_LDFLAGS = -L${top_builddir}/coeffs -L${top_builddir}/reporter -L${top_builddir}/resources -L${top_builddir}/misc ${USE_FACTORY} -L${top_builddir}/../omalloc


TESTS_ENVIRONMENT = SINGULARPATH='${top_srcdir}/../factory:${top_builddir}/../factory'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='$(top_builddir)' 


TESTS = testg testr
check_PROGRAMS = $(TESTS)

testr_SOURCES = test.cc
testg_SOURCES = test.cc

testr_LDADD = -lcoeffs -lresources -lreporter -lmisc ${FACTORY_LIBS} -lomalloc ${NTL_LIBS} ${GMP_LIBS} 
testg_LDADD = -lcoeffs_g -lresources_g -lreporter_g -lmisc_g ${FACTORY_LIBS} -lomalloc_g ${NTL_LIBS} ${GMP_LIBS} 


# testr: libcoeffs.a
# testg: libcoeffs_g.a
# gftables 

testr_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
testg_CFLAGS = ${PIPE}

testr_CXXFLAGS = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS} 
# -fimplicit-templates
testg_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fimplicit-templates
# -fdiagnostics-show-option 

testr_CPPFLAGS = -DNDEBUG -DOM_NDEBUG
testg_CPPFLAGS =

#gftables: ${top_srcdir}/../factory/gftables
#	ln -snf ${top_srcdir}/../factory/gftables ${builddir}
