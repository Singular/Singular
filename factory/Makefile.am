CXXTEMPLFLAGS      =  --no-exceptions ## -fno-implicit-templates

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} ${GMP_CFLAGS} ${NTL_CFLAGS}

lib_LIBRARIES = libfactory.a libfactory_g.a

libfactory_a_libdir = $(libdir)
libfactory_g_a_libdir = $(libdir)

libfactory_a_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${CXXTEMPLFLAGS}
libfactory_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${CXXTEMPLFLAGS}

# factory source files
SOURCES = \
		algext.cc \
		canonicalform.cc \
		cf_algorithm.cc \
		cf_binom.cc \
		cf_char.cc \
		cf_chinese.cc \
		cf_cyclo.cc \
		cf_eval.cc \
		cf_factor.cc \
		cf_factory.cc \
		cf_gcd.cc \
		cf_gcd_charp.cc \
		cf_gcd_smallp.cc \
		cf_generator.cc \
		cfGEval.cc \
		cf_globals.cc \
		cf_hnf.cc \
		cf_inline.cc \
		cf_irred.cc \
		cf_iter.cc \
		cf_iter_inline.cc \
		cf_linsys.cc \
		cf_map.cc \
		cf_map_ext.cc \
		cfNewtonPolygon.cc \
		cfModResultant.cc \
		cf_ops.cc \
		cf_primes.cc \
		cf_random.cc \
		cf_resultant.cc \
		cf_reval.cc \
		cf_switches.cc \
		cf_util.cc \
		debug.cc \
		DegreePattern.cc \
		ExtensionInfo.cc \
		facAlgExt.cc \
		facBivar.cc \
		fac_berlekamp.cc \
		fac_cantzass.cc \
		fac_distrib.cc \
		fac_ezgcd.cc \
		facFactorize.cc \
		fac_iterfor.cc \
		fac_multihensel.cc \
		fac_multivar.cc \
		fac_sqrfree.cc \
		fac_univar.cc \
		fac_util.cc \
		facFqBivar.cc \
		facFqBivarUtil.cc \
		facFqFactorize.cc \
		facFqFactorizeUtil.cc \
		facFqSquarefree.cc \
		facHensel.cc \
		facIrredTest.cc \
		fieldGCD.cc \
		ffops.cc \
		ffreval.cc \
		gf_tabutil.cc \
		gfops.cc \
		imm.cc \
		int_cf.cc \
		int_int.cc \
		int_intdiv.cc \
		int_poly.cc \
		int_pp.cc \
		int_rat.cc \
		sm_sparsemod.cc \
		sm_util.cc \
		variable.cc \
		NTLconvert.cc \
		abs_fac.cc \
		bifac.cc \
		lgs.cc \
		singext.cc \
		parseutil.cc \
		ftmpl_inst.cc

if WITH_PARSER_FOR_CANONICAL_FORM
    SOURCES +=  readcf.yy
endif

libfactory_a_SOURCES = $(SOURCES)
libfactory_g_a_SOURCES = $(SOURCES)

# factory header files
factory_headers = \
		cf_assert.h \
		canonicalform.h \
		cf_algorithm.h \
		cf_cyclo.h \
		cf_binom.h \
		cf_defs.h \
		cf_eval.h \
		cf_factory.h \
		cf_generator.h \
		cfGEval.h \
		cf_globals.h \
		cf_gcd_smallp.h \
		cf_gmp.h \
		cf_hnf.h \
		cf_irred.h \
		cf_iter.h \
		cf_map.h \
		cf_map_ext.h \
		cfNewtonPolygon.h \
		cfModResultant.h \
		cf_primes.h \
		cf_primetab.h \
		cf_random.h \
		cf_reval.h \
		cf_switches.h \
		cf_util.h \
		debug.h \
		DegreePattern.h \
		ExtensionInfo.h \
		facAlgExt.h \
		facBivar.h \
		fac_berlekamp.h \
		fac_cantzass.h \
		fac_distrib.h \
		facFactorize.h \
		fac_iterfor.h \
		fac_multivar.h \
		fac_sqrfree.h \
		fac_univar.h \
		fac_util.h \
		facFqBivar.h \
		facFqBivarUtil.h \
		facFqFactorize.h \
		facFqFactorizeUtil.h \
		facFqSquarefree.h \
		facHensel.h \
		facIrredTest.h \
		fieldGCD.h \
		ffops.h \
		ffreval.h \
		templates/ftmpl_list.h \
		templates/ftmpl_matrix.h \
		gf_tabutil.h \
		gfops.h \
		gmpext.h \
		imm.h \
		int_cf.h \
		int_int.h \
		int_poly.h \
		int_pp.h \
		int_rat.h \
		sm_sparsemod.h \
		sm_util.h \
		timing.h \
		variable.h \
		NTLconvert.h \
		bifac.h \
		bifacConfig.h \
		lgs.h \
		algext.h \
		singext.h \
		parseutil.h

noinst_HEADERS = $(factory_headers)

libfactory_includedir=$(includedir)/factory
libfactory_include_HEADERS = cplusplus.h factory.h factoryconf.h


# Documentation
include $(top_srcdir)/aminclude.am

doxysrc = $(SOURCES) parseutil.cc
doxyincl = $(factory_headers) parseutil.h

# doxygen targets
doxy:		factory.cfg $(doxysrc) $(doxyincl)
		doxygen $<

doxyclean:
		rm -rf doxygen

# the precomputed GF(q)-tables

gftablesdir=$(datadir)/gftables
dist_gftables_DATA = gftables/10201 gftables/1024 gftables/10609 gftables/11449 \
gftables/11881 gftables/121 gftables/12167 gftables/125 gftables/12769 \
gftables/128 gftables/1331 gftables/1369 gftables/14641 \
gftables/15625 gftables/16 gftables/16129 gftables/16384 \
gftables/16807 gftables/1681 gftables/169 gftables/17161 gftables/1849 \
gftables/18769 gftables/19321 gftables/19683 gftables/2048 \
gftables/2187 gftables/2197 gftables/2209 gftables/22201 \
gftables/22801 gftables/2401 gftables/243 gftables/24389 \
gftables/24649 gftables/25 gftables/256 gftables/26569 gftables/27 \
gftables/27889 gftables/2809 gftables/28561 gftables/289 \
gftables/29791 gftables/29929 gftables/3125 gftables/32 gftables/32041 \
gftables/32761 gftables/32768 gftables/343 gftables/3481 gftables/361 \
gftables/36481 gftables/3721 gftables/37249 gftables/38809 \
gftables/39601 gftables/4 gftables/4096 gftables/44521 gftables/4489 \
gftables/49 gftables/4913 gftables/49729 gftables/5041 \
gftables/50653 gftables/512 gftables/51529 gftables/52441 gftables/529 \
gftables/5329 gftables/54289 gftables/57121 gftables/58081 \
gftables/59049 gftables/6241 gftables/625 gftables/63001 \
gftables/64 gftables/6561 gftables/6859 gftables/6889 gftables/729 \
gftables/7921 gftables/8 gftables/81 gftables/8192 gftables/841 \
gftables/9 gftables/9409 gftables/961 


# Support for building GF(q)-tables
#
# gengftables needs libfactory.a and factory.h AND is needed for
# generating gftable but the tables are not necessarily needed to
# compile and use libfactory (though it will be a lot slower).

EXTRA_PROGRAMS = gengftables 
gengftables_SOURCES = gengftables-conway.cc
gengftables_LDADD = -L${builddir} -lfactory $(GMP_LIBS) $(NTL_LIBS)


# These files listed below are not used anywhere but are included in
# the distribution. So they will be tacked on to EXTRA_DIST.

templatesrc =	templates/ftmpl_array.cc \
		templates/ftmpl_factor.cc \
		templates/ftmpl_functions.h \
		templates/ftmpl_list.cc \
		templates/ftmpl_matrix.cc

templateincl =	templates/ftmpl_array.h \
		templates/ftmpl_factor.h \
		templates/ftmpl_list.h \
		templates/ftmpl_matrix.h

# header templates
hdrtemplsrc = 	factoryconf.template \
		factory.template

# FTE source files
ftestsrc =	ftest/ftest_io.cc \
		ftest/ftest_util.cc \
		ftest/ntl_util.cc

# FTE header files
ftestincl =	ftest/ftest_io.h \
		ftest/ftest_util.h \
		ftest/ntl_util.h

ftestm4src =	ftest/commonden.m4 \
		ftest/degree.m4 \
		ftest/deriv.m4 \
		ftest/divides.m4 \
		ftest/divrem.m4 \
		ftest/extgcd.m4 \
		ftest/factorize.m4 \
		ftest/fbinops.m4 \
		ftest/feval.m4 \
		ftest/gcd.m4 \
		ftest/gcd.ntl.m4 \
		ftest/insert.m4 \
		ftest/norm.m4 \
		ftest/resultant.m4 \
		ftest/revert.m4 \
		ftest/sqrfree.m4 \
		ftest/size.m4 \
		ftest/totaldegree.m4

ftestdistfiles = \
		ftest/ftest_util.m4

EXTRA_DIST =	test_install.cc \
		$(templatesrc) $(templateincl) $(hdrtemplsrc) \
		$(ftestsrc) $(ftestincl) $(ftestm4src) $(ftestdistfiles) \
		doxygen.cfg factory.cfg \
		examples/application.cc \
		examples/factorize.cc examples/gcd.cc \
		bin/folding.el bin/fold-docu.el \
		bin/makeheader bin/gen-readcf 

##################################################
# autogenerated sources

noinst_PROGRAMS = cplusplus

BUILT_SOURCES = cplusplus.h factory.h factoryconf.h readcf.cc 

CLEANFILES = $(BUILT_SOURCES)

cplusplus_SOURCES = cplusplus.cc

cplusplus.h: cplusplus
	${builddir}/cplusplus > ${builddir}/cplusplus.h

.template.h:
	${srcdir}/bin/makeheader $< $@

# factory/gfops.cc has the gftables path hardcoded in, so we create a
# symlink. Remove this rule once gfopts.cc has been fixed. Note that
# check-local would run in parallel with check, so we can't use it
# here!
all-local:
	[ -d ${builddir}/gftables ] || ln -s ${srcdir}/gftables ${builddir}

####################################################
## Test program
##
TESTS = test-g test-r
check_PROGRAMS = $(TESTS)

TESTSSOURCES = test.cc 

test_g_SOURCES = $(TESTSSOURCES)
test_g_CFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long -fomit-frame-pointer --no-rtti #ftmpl_inst.o
test_g_LDADD = -L${builddir} -lfactory_g $(GMP_LIBS) $(NTL_LIBS)

test_r_SOURCES = $(TESTSSOURCES)
test_r_CFLAGS = -O3 -fomit-frame-pointer --no-rtti #ftmpl_inst.o
test_r_LDADD = -L${builddir} -lfactory $(GMP_LIBS) $(NTL_LIBS)

