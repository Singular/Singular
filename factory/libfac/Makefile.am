noinst_LTLIBRARIES=libfac.la libfac_g.la

CXXTEMPLFLAGS =  --no-exceptions ## -fno-implicit-templates

libfac_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libfac_g_la_CFLAGS = ${PIPE}

libfac_la_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
libfac_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} -I${builddir} -I${srcdir}/..  -I${builddir}/.. -I${srcdir}/factor  -I${builddir}/factor -I${srcdir}/charset -I${builddir}/charset ${GMP_CFLAGS} ${NTL_CFLAGS}

libfac_la_CPPFLAGS   = ${AM_CPPFLAGS} -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H
libfac_g_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H

FACTORSRC1 =	factor/SqrFree.cc factor/Factor.cc \
		factor/MVMultiHensel.cc factor/Truefactor.cc \
		factor/helpstuff.cc factor/homogfactor.cc factor/version.cc

FACTORSRC2 =	factor/tmpl_inst.cc  

FACTORSRC = 	$(FACTORSRC1) $(FACTORSRC2) 


CHARSETSRC1 =	charset/csutil.cc charset/charset.cc charset/reorder.cc charset/alg_factor.cc charset/algfactor.cc

CHARSETSRC = 	$(CHARSETSRC1)


SOURCES  = $(FACTORSRC) $(CHARSETSRC)
libfac_la_SOURCES   = $(SOURCES)
libfac_g_la_SOURCES = $(SOURCES)


libfac_includedir=$(includedir)/factory/libfac

libfac_include_HEADERS   = factor.h 



