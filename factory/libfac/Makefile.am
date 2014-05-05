ACLOCAL_AMFLAGS = -I ../../m4

noinst_LTLIBRARIES=libfac.la 

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir}/.. -I${builddir}/.. -I${srcdir}/../include  -I${builddir}/../include -I${srcdir}/factor  -I${builddir}/factor -I${srcdir}/charset -I${builddir}/charset ${GMP_CFLAGS} ${NTL_CFLAGS}

FACTORSRC1 =	factor/helpstuff.cc factor/version.c

FACTORSRC2 =	factor/tmpl_inst.cc  

FACTORSRC = 	$(FACTORSRC1) $(FACTORSRC2) 


CHARSETSRC1 =	charset/csutil.cc charset/charset.cc charset/reorder.cc charset/alg_factor.cc charset/algfactor.cc

CHARSETSRC = 	$(CHARSETSRC1)


SOURCES  = $(FACTORSRC) $(CHARSETSRC)

noinst_HEADERS = factor/helpstuff.h factor/debug.h factor/interrupt.h factor/timing.h \
    factor/tmpl_inst.h \
    charset/csutil.h charset/charset.h charset/reorder.h charset/alg_factor.h charset/algfactor.h 

libfac_la_SOURCES   = $(SOURCES)

libfac_includedir=$(includedir)/factory/libfac
libfac_include_HEADERS   = libfac.h 

EXTRA_DIST = test.cc testcs.cc header.tpl \
      charset/tests
