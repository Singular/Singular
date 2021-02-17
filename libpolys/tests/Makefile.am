ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} \
${FACTORY_INCLUDES} ${RESOURCES_INCLUDES} ${OMALLOC_INCLUDES} \
${FLINT_CFLAGS} ${GMP_CPPFLAGS}

TESTS = simple_test \
    coeffs_test \
    polys_test \
    rings_test

check_PROGRAMS = $(TESTS)

nodist_simple_test_SOURCES = simple_test_runner.cpp

nodist_coeffs_test_SOURCES = coeffs_test_runner.cpp
nodist_polys_test_SOURCES = polys_test_runner.cpp
nodist_rings_test_SOURCES = rings_test_runner.cpp

coeffs_test_LDADD = ${top_builddir}/libpolys.la \
$(FACTORY_LIBS) $(RESOURCES_LIBS) $(OMALLOC_LIBS) \
$(FLINT_LIBS) $(GMP_LIBS) $(NTL_LIBS)

simple_test_LDADD = $(coeffs_test_LDADD)
polys_test_LDADD = $(coeffs_test_LDADD)
rings_test_LDADD = $(polys_test_LDADD)

BUILT_SOURCES = gftables MOD \
        simple_test_runner.cpp coeffs_test_runner.cpp polys_test_runner.cpp rings_test_runner.cpp

gftables: ${top_srcdir}/../factory/gftables
	ln -snf ${top_srcdir}/../factory/gftables ${builddir}

MOD:
	if [ -d "${top_builddir}/libpolys/.libs" ]; then \
	  ln -snf ${top_builddir}/libpolys/.libs/ ${abs_builddir}/MOD; \
	fi

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
