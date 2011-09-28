CXXTEMPLFLAGS = -O0 --no-exceptions ##  ## -fno-implicit-templates

if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/../factory
else
  USE_FACTORY =
endif

## for testing...
AM_LDFLAGS = -L${top_builddir}/polys -L${top_builddir}/coeffs -L${top_builddir}/reporter -L${top_builddir}/resources -L${top_builddir}/misc ${USE_FACTORY} -L${top_builddir}/../omalloc 


AM_CFLAGS = -g -Wextra -Wall -Wno-long-long ${PIPE}
## -pedantic

AM_CXXFLAGS = $(AM_CFLAGS) ${CXXTEMPLFLAGS}

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} ${FACTORY_CFLAGS} ${GMP_CFLAGS} ${NTL_CFLAGS} 


TESTS = simple_test coeffs_test rings_test polys_test
check_PROGRAMS = $(TESTS)

simple_test_SOURCES = simple_test_runner.cpp
coeffs_test_SOURCES = coeffs_test_runner.cpp
polys_test_SOURCES = polys_test_runner.cpp
rings_test_SOURCES = rings_test_runner.cpp

polys_test_LDFLAGS = ${AM_LDFLAGS} ${USEPPROCSDYNAMICLDFLAGS}
rings_test_LDFLAGS = $(polys_test_LDFLAGS)

coeffs_test_LDADD = -lcoeffs_g -lresources_g -lreporter_g -lmisc_g ${FACTORY_LIBS} -lomalloc_g ${NTL_LIBS} ${GMP_LIBS} 
polys_test_LDADD = -lpolys_g ${USEPPROCSDYNAMICLD} $(coeffs_test_LDADD)
rings_test_LDADD = $(polys_test_LDADD)


BUILT_SOURCES = gftables MOD \
	$(simple_test_SOURCES) $(coeffs_test_SOURCES) $(rings_test_SOURCES) $(polys_test_SOURCES)

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

EXTRA_DIST = simple_test.h coeffs_test.h polys_test.h rings_test.h common.h 		\
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
