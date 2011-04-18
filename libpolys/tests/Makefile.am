CXXTEMPLFLAGS = --no-exceptions ## -fno-implicit-templates

## for testing...
AM_LDFLAGS = -L${top_builddir}/coeffs -L${top_builddir}/reporter -L${top_builddir}/resources -L${top_builddir}/misc -L${top_builddir}/../omalloc 


AM_CFLAGS = -g -Wextra -Wall -Wno-long-long ${PIPE}
## -pedantic

AM_CXXFLAGS = $(AM_CFLAGS) ${CXXTEMPLFLAGS}

AM_CPPFLAGS = -DHAVE_CONFIG_H

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${srcdir} ${GMP_CFLAGS}

TESTS = simple_test coeffs_test
check_PROGRAMS = $(TESTS)

simple_test_SOURCES = simple_test_runner.cpp

coeffs_test_SOURCES = coeffs_test_runner.cpp

coeffs_test_LDADD = -lcoeffs_g ${GMP_LIBS} -lresources_g -lreporter_g -lmisc -lomalloc_debug


BUILT_SOURCES = $(simple_test_SOURCES) $(coeffs_test_SOURCES)


# How to generate the test simple_test_runner
simple_test_runner.cpp: simple_test.h
	${srcdir}/cxxtestgen.pl --no-eh -o $@ --error-printer $^

coeffs_test_runner.cpp: coeffs_test.h
	${srcdir}/cxxtestgen.pl --no-eh -o $@ --error-printer $^
