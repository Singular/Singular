CXXTEMPLFLAGS =-ftrapv ## -O0 ## -fno-implicit-templates

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} -I${top_srcdir}/../factory/include -I${top_builddir}/../factory/include ${FACTORY_CFLAGS} ${GMP_CFLAGS} ${NTL_CFLAGS} 

if WANT_DEBUG
TESTS = simple_test \
    coeffs_test_s_r coeffs_test_s_g coeffs_test_d_r coeffs_test_d_g \
    polys_test_s_r polys_test_s_g polys_test_d_r polys_test_d_g \
    rings_test_s_r rings_test_s_g rings_test_d_r rings_test_d_g
else
TESTS = simple_test \
    coeffs_test_s_r coeffs_test_d_r \
    polys_test_s_r polys_test_d_r \
    rings_test_s_r rings_test_d_r
endif

check_PROGRAMS = $(TESTS)

nodist_simple_test_SOURCES = simple_test_runner.cpp

nodist_coeffs_test_s_r_SOURCES = coeffs_test_runner.cpp
nodist_polys_test_s_r_SOURCES = polys_test_runner.cpp
nodist_rings_test_s_r_SOURCES = rings_test_runner.cpp

nodist_coeffs_test_s_g_SOURCES = coeffs_test_runner.cpp
nodist_polys_test_s_g_SOURCES = polys_test_runner.cpp
nodist_rings_test_s_g_SOURCES = rings_test_runner.cpp


nodist_coeffs_test_d_r_SOURCES = coeffs_test_runner.cpp
nodist_polys_test_d_r_SOURCES = polys_test_runner.cpp
nodist_rings_test_d_r_SOURCES = rings_test_runner.cpp

nodist_coeffs_test_d_g_SOURCES = coeffs_test_runner.cpp
nodist_polys_test_d_g_SOURCES = polys_test_runner.cpp
nodist_rings_test_d_g_SOURCES = rings_test_runner.cpp



RELEASECFLAGS = -O3 -fomit-frame-pointer ${PIPE}
RELEASECXXFLAGS = -O3 -fomit-frame-pointer ${PIPE}
RELEASECPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG

coeffs_test_s_r_CFLAGS =${RELEASECFLAGS} 
polys_test_s_r_CFLAGS = ${RELEASECFLAGS}
rings_test_s_r_CFLAGS = ${RELEASECFLAGS}

coeffs_test_d_r_CFLAGS =${RELEASECFLAGS} 
polys_test_d_r_CFLAGS = ${RELEASECFLAGS}
rings_test_d_r_CFLAGS = ${RELEASECFLAGS}


coeffs_test_s_r_CXXFLAGS =${RELEASECXXFLAGS} 
polys_test_s_r_CXXFLAGS = ${RELEASECXXFLAGS}
rings_test_s_r_CXXFLAGS = ${RELEASECXXFLAGS}

coeffs_test_d_r_CXXFLAGS =${RELEASECXXFLAGS} 
polys_test_d_r_CXXFLAGS = ${RELEASECXXFLAGS}
rings_test_d_r_CXXFLAGS = ${RELEASECXXFLAGS}


coeffs_test_s_r_CPPFLAGS =${RELEASECPPFLAGS} 
polys_test_s_r_CPPFLAGS = ${RELEASECPPFLAGS}
rings_test_s_r_CPPFLAGS = ${RELEASECPPFLAGS}

coeffs_test_d_r_CPPFLAGS =${RELEASECPPFLAGS} 
polys_test_d_r_CPPFLAGS = ${RELEASECPPFLAGS}
rings_test_d_r_CPPFLAGS = ${RELEASECPPFLAGS}


###  -pedantic
DEBUGCFLAGS = $(AM_CFLAGS) -g -Wextra -Wall -Wno-long-long ${PIPE}
DEBUGCXXFLAGS = $(AM_CXXFLAGS) ${DEBUGCFLAGS} ${CXXTEMPLFLAGS}
DEBUGCPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H

coeffs_test_s_g_CFLAGS =${DEBUGCFLAGS} 
polys_test_s_g_CFLAGS = ${DEBUGCFLAGS}
rings_test_s_g_CFLAGS = ${DEBUGCFLAGS}

coeffs_test_d_g_CFLAGS =${DEBUGCFLAGS} 
polys_test_d_g_CFLAGS = ${DEBUGCFLAGS}
rings_test_d_g_CFLAGS = ${DEBUGCFLAGS}

coeffs_test_s_g_CXXFLAGS =${DEBUGCXXFLAGS} 
polys_test_s_g_CXXFLAGS = ${DEBUGCXXFLAGS}
rings_test_s_g_CXXFLAGS = ${DEBUGCXXFLAGS}

coeffs_test_d_g_CXXFLAGS =${DEBUGCXXFLAGS} 
polys_test_d_g_CXXFLAGS = ${DEBUGCXXFLAGS}
rings_test_d_g_CXXFLAGS = ${DEBUGCXXFLAGS}

coeffs_test_d_g_CPPFLAGS =${DEBUGCPPFLAGS} 
polys_test_d_g_CPPFLAGS = ${DEBUGCPPFLAGS}
rings_test_d_g_CPPFLAGS = ${DEBUGCPPFLAGS}

coeffs_test_s_g_CPPFLAGS =${DEBUGCPPFLAGS} 
polys_test_s_g_CPPFLAGS = ${DEBUGCPPFLAGS}
rings_test_s_g_CPPFLAGS = ${DEBUGCPPFLAGS}


if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/../factory
else
  USE_FACTORY =
endif

AM_LDFLAGS = -L${top_builddir}/polys -L${top_builddir}/coeffs -L${top_builddir}/reporter -L${top_builddir}/misc ${USE_FACTORY} -L${top_builddir}/../omalloc -L${top_builddir}/../findexec


coeffs_test_s_r_LDFLAGS = ${AM_LDFLAGS} -static 
coeffs_test_s_g_LDFLAGS = ${AM_LDFLAGS} -static 


polys_test_s_r_LDFLAGS = ${AM_LDFLAGS} ${USEPPROCSDYNAMICLDFLAGS} -static
rings_test_s_r_LDFLAGS = $(polys_test_s_r_LDFLAGS)

polys_test_s_g_LDFLAGS = ${AM_LDFLAGS} ${USEPPROCSDYNAMICLDFLAGS} -static
rings_test_s_g_LDFLAGS = $(polys_test_s_g_LDFLAGS)

polys_test_d_r_LDFLAGS = ${AM_LDFLAGS} ${USEPPROCSDYNAMICLDFLAGS}
rings_test_d_r_LDFLAGS = $(polys_test_d_r_LDFLAGS)

polys_test_d_g_LDFLAGS = ${AM_LDFLAGS} ${USEPPROCSDYNAMICLDFLAGS}
rings_test_d_g_LDFLAGS = $(polys_test_d_g_LDFLAGS)


coeffs_test_s_r_LDADD = -lcoeffs -lreporter -lmisc ${FACTORY_LIBS} -lomalloc -lfindexec ${NTL_LIBS} ${GMP_LIBS} 
polys_test_s_r_LDADD = -lpolys -lcoeffs -lreporter -lmisc ${USEPPROCSDYNAMICLD} ${FACTORY_LIBS} -lomalloc -lfindexec ${NTL_LIBS} ${GMP_LIBS} 
rings_test_s_r_LDADD = $(polys_test_s_r_LDADD)

coeffs_test_s_g_LDADD = -lcoeffs_g -lreporter_g -lmisc_g ${FACTORY_LIBS} -lomalloc_g -lfindexec_g ${NTL_LIBS} ${GMP_LIBS} 
polys_test_s_g_LDADD = -lpolys_g -lcoeffs_g -lreporter_g -lmisc_g ${USEPPROCSDYNAMICLD} ${FACTORY_LIBS} -lomalloc_g -lfindexec_g ${NTL_LIBS} ${GMP_LIBS} 
rings_test_s_g_LDADD = $(polys_test_s_g_LDADD)

coeffs_test_d_r_LDADD = -lcoeffs -lreporter -lmisc ${FACTORY_LIBS} -lomalloc -lfindexec ${NTL_LIBS} ${GMP_LIBS} 
polys_test_d_r_LDADD = -lpolys -lcoeffs -lreporter -lmisc ${USEPPROCSDYNAMICLD} ${FACTORY_LIBS} -lomalloc -lfindexec ${NTL_LIBS} ${GMP_LIBS}
rings_test_d_r_LDADD = $(polys_test_d_r_LDADD)

coeffs_test_d_g_LDADD = -lcoeffs_g -lreporter_g -lmisc_g ${FACTORY_LIBS} -lomalloc_g -lfindexec_g ${NTL_LIBS} ${GMP_LIBS} 
polys_test_d_g_LDADD = -lpolys_g -lcoeffs_g -lreporter_g -lmisc_g ${USEPPROCSDYNAMICLD} ${FACTORY_LIBS} -lomalloc_g -lfindexec_g ${NTL_LIBS} ${GMP_LIBS}
rings_test_d_g_LDADD = $(polys_test_d_g_LDADD)







BUILT_SOURCES = gftables MOD \
        simple_test_runner.cpp coeffs_test_runner.cpp polys_test_runner.cpp rings_test_runner.cpp

gftables: ${top_srcdir}/../factory/gftables
	ln -snf ${top_srcdir}/../factory/gftables ${builddir}

MOD: ${top_builddir}/polys/.libs
	ln -snf ${top_builddir}/polys/.libs/ ${builddir}/MOD

# How to generate *_runner.cpp 
simple_test_runner.cpp: simple_test.h
	${srcdir}/cxxtestgen.pl --no-eh -o $@ --error-printer ${srcdir}/simple_test.h

coeffs_test_runner.cpp: coeffs_test.h
	${srcdir}/cxxtestgen.pl --no-eh -o $@ --error-printer ${srcdir}/coeffs_test.h

polys_test_runner.cpp: polys_test.h
	${srcdir}/cxxtestgen.pl --no-eh -o $@ --error-printer ${srcdir}/polys_test.h

rings_test_runner.cpp: rings_test.h
	${srcdir}/cxxtestgen.pl --no-eh -o $@ --error-printer ${srcdir}/rings_test.h

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = cxxtestgen.pl simple_test.h coeffs_test.h polys_test.h rings_test.h common.h \
    cxxtest/Descriptions.h       cxxtest/ParenPrinter.h      cxxtest/TestListener.h	\
    cxxtest/DummyDescriptions.h  cxxtest/QtGui.h             cxxtest/TestRunner.h	\
    cxxtest/ErrorFormatter.h     cxxtest/RealDescriptions.h  cxxtest/TestSuite.h	\
    cxxtest/ErrorPrinter.h       cxxtest/SelfTest.h          cxxtest/TestTracker.h	\
    cxxtest/Flags.h              cxxtest/StdHeaders.h        cxxtest/ValueTraits.h	\
    cxxtest/GlobalFixture.h      cxxtest/StdioFilePrinter.h  cxxtest/Win32Gui.h		\
    cxxtest/Gui.h                cxxtest/StdioPrinter.h      cxxtest/X11Gui.h		\
    cxxtest/LinkedList.h         cxxtest/StdValueTraits.h    cxxtest/YesNoRunner.h	\
    cxxtest/Mock.h               cxxtest/TeeListener.h					\
    cxxtest/Descriptions.cpp     cxxtest/LinkedList.cpp      cxxtest/TestSuite.cpp	\
    cxxtest/DummyDescriptions.cpp cxxtest/RealDescriptions.cpp  cxxtest/TestTracker.cpp \
    cxxtest/GlobalFixture.cpp    cxxtest/Root.cpp            cxxtest/ValueTraits.cpp
