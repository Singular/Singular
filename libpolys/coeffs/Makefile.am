ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} \
$(FACTORY_INCLUDES) $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${NTL_CPPFLAGS} $(FLINT_CFLAGS) ${GMP_CPPFLAGS}

# noinst???
noinst_LTLIBRARIES = libcoeffs.la
###### libcoeffsdir = $(libdir)/singular
# noinst_HEADERS= \
#	gnumpc.h gnumpfl.h longrat.h modulop.h ffields.h \
#	rintegers.h rmodulo2m.h rmodulon.h shortfl.h \
#	mpr_complex.h mpr_global.h

SOURCES = \
  numbers.cc rintegers.cc rmodulo2m.cc rmodulon.cc shortfl.cc \
  gnumpc.cc gnumpfl.cc longrat.cc longrat0.cc ffields.cc \
  modulop.cc mpr_complex.cc ntupel.cc \
  bigintmat.cc  \
  flintcf_Qrat.cc flintcf_Q.cc flintcf_Zn.cc

libcoeffs_la_SOURCES   = $(SOURCES)

libcoeffs_la_includedir  =$(includedir)/singular/coeffs
libcoeffs_la_include_HEADERS = \
  coeffs.h numbers.h si_gmp.h gnumpc.h gnumpfl.h longrat.h modulop.h ffields.h rintegers.h rmodulo2m.h rmodulon.h \
  shortfl.h mpr_complex.h mpr_global.h numstats.h ntupel.h \
  bigintmat.h Enumerator.h \
  flintcf_Qrat.h flintcf_Q.h flintcf_Zn.h

libcoeffs_la_LIBADD = ${top_builddir}/reporter/libreporter.la ${top_builddir}/misc/libmisc.la \
$(FACTORY_LIBS) $(RESOURCES_LIBS) $(OMALLOC_LIBS) \
$(FLINT_LIBS) $(GMP_LIBS) $(NTL_LIBS)

libcoeffs_la_LDFLAGS = $(SINGULAR_LDFLAGS)

TESTS_ENVIRONMENT = SINGULARPATH='${top_srcdir}/../factory:${top_builddir}/../factory'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='$(top_builddir)'

TESTS = test
check_PROGRAMS = $(TESTS)

test_SOURCES = test.cc
test_LDADD   = libcoeffs.la $(libcoeffs_la_LIBADD)

EXTRA_DIST = rintegers2.cc rintegers3.cc
