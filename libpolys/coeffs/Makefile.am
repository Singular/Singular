ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} -I${top_srcdir}/../factory/include -I${top_builddir}/../factory/include ${FACTORY_CFLAGS} ${GMP_CFLAGS} ${NTL_CFLAGS}  -DHAVE_CONFIG_H

libcoeffs_LTLIBRARIES = libcoeffs.la 
libcoeffsdir = $(libdir)/singular

# noinst_HEADERS= \
#	gnumpc.h gnumpfl.h longrat.h modulop.h ffields.h \
#	rintegers.h rmodulo2m.h rmodulon.h shortfl.h \
#	mpr_complex.h mpr_global.h


SOURCES = \
	numbers.cc rintegers.cc rmodulo2m.cc rmodulon.cc shortfl.cc \
	gnumpc.cc gnumpfl.cc longrat.cc longrat0.cc ffields.cc \
	modulop.cc mpr_complex.cc \
  bigintmat.cc  AE.cc OPAE.cc AEp.cc OPAEp.cc AEQ.cc OPAEQ.cc

libcoeffs_la_SOURCES   = $(SOURCES)

libcoeffs_includedir  =$(includedir)/singular/coeffs
libcoeffs_include_HEADERS = coeffs.h numbers.h si_gmp.h gnumpc.h gnumpfl.h longrat.h modulop.h ffields.h rintegers.h rmodulo2m.h rmodulon.h shortfl.h mpr_complex.h mpr_global.h \
  bigintmat.h Enumerator.h AE.h OPAE.h AEp.h OPAEp.h AEQ.h OPAEQ.h

TESTS_ENVIRONMENT = SINGULARPATH='${top_srcdir}/../factory:${top_builddir}/../factory'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='$(top_builddir)' 

TESTS = test

check_PROGRAMS = $(TESTS)

if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/../factory
else
  USE_FACTORY =
endif

AM_LDFLAGS = -L${top_builddir}/coeffs -L${top_builddir}/reporter -L${top_builddir}/misc ${USE_FACTORY} -L${top_builddir}/../omalloc -L${top_builddir}/../resources

test_SOURCES = test.cc
test_LDADD   = -lcoeffs -lreporter -lmisc ${FACTORY_LIBS} -lomalloc -lresources ${NTL_LIBS} ${GMP_LIBS} 
