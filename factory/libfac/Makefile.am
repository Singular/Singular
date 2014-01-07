ACLOCAL_AMFLAGS = -I ../../m4

# If Want_Debug
#   Lib_G=Libfac_G.La
# Else
#   Lib_G=
# Endif

noinst_LTLIBRARIES=libfac.la 
# ${LIB_G}

#CXXTEMPLFLAGS =-ftrapv  ## -fno-implicit-templates

#libfac_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
#libfac_g_la_CFLAGS = ${PIPE}

#libfac_la_CXXFLAGS   = -O3 -fomit-frame-pointer ${PIPE} ${CXXTEMPLFLAGS}
#libfac_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} -I${builddir} -I${srcdir}/.. -I${builddir}/.. -I${srcdir}/../include  -I${builddir}/../include -I${srcdir}/factor  -I${builddir}/factor -I${srcdir}/charset -I${builddir}/charset ${GMP_CFLAGS} ${NTL_CFLAGS}  -DHAVE_CONFIG_H

# libfac_la_CPPFLAGS   = ${AM_CPPFLAGS} -DSING_NDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H
# libfac_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H

# libfac_la_LDFLAGS    = -release ${PACKAGE_VERSION}

FACTORSRC1 =	factor/SqrFree.cc factor/Factor.cc \
		factor/MVMultiHensel.cc factor/Truefactor.cc \
		factor/helpstuff.cc factor/homogfactor.cc factor/version.c

FACTORSRC2 =	factor/tmpl_inst.cc  

FACTORSRC = 	$(FACTORSRC1) $(FACTORSRC2) 


CHARSETSRC1 =	charset/csutil.cc charset/charset.cc charset/reorder.cc charset/alg_factor.cc charset/algfactor.cc

CHARSETSRC = 	$(CHARSETSRC1)


SOURCES  = $(FACTORSRC) $(CHARSETSRC)

noinst_HEADERS = factor/SqrFree.h factor/Factor.h \
    factor/MVMultiHensel.h factor/Truefactor.h \
    factor/helpstuff.h factor/homogfactor.h \
    factor/class.h factor/debug.h factor/interrupt.h factor/timing.h \
    factor/tmpl_inst.h \
    charset/csutil.h charset/charset.h charset/reorder.h charset/alg_factor.h charset/algfactor.h 

libfac_la_SOURCES   = $(SOURCES)
# libfac_g_la_SOURCES = $(SOURCES)


libfac_includedir=$(includedir)/factory/libfac

libfac_include_HEADERS   = libfac.h 

EXTRA_DIST = factor/class.cc \
      factor/test.cc test.cc testcs.cc header.tpl \
      charset/tests bin \
      ChangeLog 00README 



