CXXTEMPLFLAGS = -O0 --no-exceptions ## -fno-implicit-templates

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

AM_CPPFLAGS = -DHAVE_CONFIG_H

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} ${GMP_CFLAGS} ${NTL_CFLAGS} ${FACTORY_CFLAGS}


TESTS = simple_test coeffs_test rings_test polys_test
check_PROGRAMS = $(TESTS)

simple_test_SOURCES = simple_test_runner.cpp
coeffs_test_SOURCES = coeffs_test_runner.cpp gftables
polys_test_SOURCES = polys_test_runner.cpp gftables
rings_test_SOURCES = rings_test_runner.cpp gftables



polys_test_LDFLAGS = ${AM_LDFLAGS} ${USEPPROCSDYNAMICLDFLAGS}
rings_test_LDFLAGS = $(polys_test_LDFLAGS)

coeffs_test_LDADD = -lcoeffs_g ${GMP_LIBS} -lresources_g -lreporter_g -lmisc_g ${FACTORY_LIBS} ${NTL_LIBS} -lomalloc_g
polys_test_LDADD = -lpolys_g ${USEPPROCSDYNAMICLD} $(coeffs_test_LDADD)
rings_test_LDADD = $(polys_test_LDADD)

noinst_SCRIPTS = p_Procs_FieldGeneral.so \
		 p_Procs_FieldIndep.so \
		 p_Procs_FieldQ.so \
		 p_Procs_FieldZp.so

gftables: ${top_srcdir}/../factory/gftables
	ln -snf ${top_srcdir}/../factory/gftables ${builddir}

p_Procs_FieldGeneral.so:	${top_builddir}/polys/.libs/p_Procs_FieldGeneral.so
				ln -snf ${top_builddir}/polys/.libs/p_Procs_FieldGeneral.so ${builddir}

p_Procs_FieldIndep.so:	${top_builddir}/polys/.libs/p_Procs_FieldIndep.so
				ln -snf ${top_builddir}/polys/.libs/p_Procs_FieldIndep.so ${builddir}

p_Procs_FieldQ.so:	${top_builddir}/polys/.libs/p_Procs_FieldQ.so
				ln -snf ${top_builddir}/polys/.libs/p_Procs_FieldQ.so ${builddir}

p_Procs_FieldZp.so:	${top_builddir}/polys/.libs/p_Procs_FieldZp.so
				ln -snf ${top_builddir}/polys/.libs/p_Procs_FieldZp.so ${builddir}

BUILT_SOURCES = $(simple_test_SOURCES) $(coeffs_test_SOURCES) $(rings_test_SOURCES) $(polys_test_SOURCES)


# How to generate *_runner.cpp 
simple_test_runner.cpp: simple_test.h
	${srcdir}/cxxtestgen.pl --no-eh -o $@ --error-printer ${srcdir}/simple_test.h

coeffs_test_runner.cpp: coeffs_test.h
	${srcdir}/cxxtestgen.pl --no-eh -o $@ --error-printer ${srcdir}/coeffs_test.h

polys_test_runner.cpp: polys_test.h
	${srcdir}/cxxtestgen.pl --no-eh -o $@ --error-printer ${srcdir}/polys_test.h

rings_test_runner.cpp: rings_test.h
	${srcdir}/cxxtestgen.pl --no-eh -o $@ --error-printer ${srcdir}/rings_test.h
