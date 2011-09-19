CXXTEMPLFLAGS      =  --no-exceptions ## -fno-implicit-templates

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} ${GMP_CFLAGS} ${NTL_CFLAGS}

lib_LIBRARIES = libfactory.a libfactory_g.a

libfactory_a_includedir=$(includedir)/factory
libfactory_g_a_includedir=$(includedir)/factory

libfactory_a_libdir=$(libdir)/factory
libfactory_g_a_libdir=$(libdir)/factory

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
		ftmpl_inst.cc \
		factory.h factoryconf.h cplusplus.h \
		readcf.yy


libfactory_a_SOURCES = $(SOURCES)
libfactory_g_a_SOURCES = $(SOURCES)

# factory header files
factory_headers = \
		cf_assert.h \
		canonicalform.h \
		cf_algorithm.h \
		cf_binom.h \
		cf_defs.h \
		cf_eval.h \
		cf_factory.h \
		cf_generator.h \
		cfGEval.h \
		cf_globals.h \
		cf_gcd_smallp.h \
		cf_hnf.h \
		cf_irred.h \
		cf_iter.h \
		cf_map.h \
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
		singext.h

noinst_HEADERS = $(factory_headers)

# Documentation
include $(top_srcdir)/aminclude.am

doxysrc = $(SOURCES) parseutil.cc
doxyincl = $(factory_headers) parseutil.h

# doxygen targets
doxy:		factory.cfg $(doxysrc) $(doxyincl)
		doxygen $<

doxyclean:
		rm -rf doxygen

# Support for building GF(q)-tables

templatesrc =	\
		ftmpl_array.cc \
		ftmpl_factor.cc \
		ftmpl_functions.h \
		ftmpl_list.cc \
		ftmpl_matrix.cc
templateincl =	\
		ftmpl_array.h \
		ftmpl_factor.h \
		ftmpl_list.h \
		ftmpl_matrix.h

# TODO:
# gengftables needs libfactory.a and factory.h AND is needed for generating gftable...
# somehow the current deps. seem to be broken... 
# moreover factory.h needs a lot more generated headers, which wouldn't be installed yet

EXTRA_PROGRAMS = gengftables cplusplus


gengftables_SOURCES = gengftables-conway.cc

gengftables_LDADD = -L${builddir} -lfactory $(GMP_LIBS) $(NTL_LIBS)

gftablesdir=$(datadir)/gftables
gftables_DATA = gftables/10201 gftables/1024 gftables/10609 gftables/11449 \
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


gftables: gengftables
		@if test -x ${srcdir}/gftables; then \
			if test ! -x ${builddir}/gftables; then \
			  ln -snf ${srcdir}/gftables ${builddir}; \
			fi; \
		else \
			echo "This command will take quite a long time to execute;"; \
			echo "maybe you want to get the tables directly via ftp (see"; \
			echo "the INSTALL file)."; \
			${builddir}/gengftables; \
		fi

.template.h:
	${srcdir}/bin/makeheader $< $@

BUILT_SOURCES = cplusplus.h factory.h factoryconf.h readcf.cc


libfactory_a_include_HEADERS=$(BUILT_SOURCES)
libfactory_g_a_include_HEADERS=$(BUILT_SOURCES)

# These files listed below are not used anywhere but are included in
# the distribution. So they will be tacked on to EXTRA_DIST.

# header templates
hdrtemplsrc = \
		factoryconf.template \
		factory.template

# FTE source files
ftestsrc =	\
		ftest_io.cc \
		ftest_util.cc \
		ntl_util.cc

# FTE header files
ftestincl =	\
		ftest_io.h \
		ftest_util.h \
		ntl_util.h

# FTE m4 source files (exported for `GNUmakefile' in `ftest/').
# The variable `$(ftestm4develsrc)' may be already initialized
# from the development `GNUmakefile'.
ftestm4src =	$(ftestm4develsrc) \
		commonden.m4 \
		degree.m4 \
		deriv.m4 \
		divides.m4 \
		divrem.m4 \
		extgcd.m4 \
		factorize.m4 \
		fbinops.m4 \
		feval.m4 \
		gcd.m4 \
		gcd.ntl.m4 \
		insert.m4 \
		norm.m4 \
		resultant.m4 \
		revert.m4 \
		sqrfree.m4 \
		size.m4 \
		totaldegree.m4

# other files from the FTE
ftestdistfiles = \
		ftest_util.m4

# gengftables.cc gengftables-conway.cc 
EXTRA_DIST =	test_install.cc \
		$(templatesrc) $(templateincl) $(hdrtemplsrc) \
		$(ftestsrc) $(ftestincl) $(ftestm4src) $(ftestdistfiles) \
		doxygen.cfg factory.cfg gen_cf_gmp.template \
		examples/application.cc \
		examples/factorize.cc examples/gcd.cc \
		bin/folding.el bin/fold-docu.el \
		bin/makeheader bin/gen-readcf $(gftables_DATA)

CLEANFILES = factory.h factoryconf.h cplusplus.h readcf.cc



cplusplus_SOURCES = cplusplus.cc

cplusplus.h: cplusplus
	${builddir}/cplusplus > ${builddir}/cplusplus.h

cf_gmp.h: cplusplus.h

#ftmpl_inst.o:	ftmpl_inst.cc factoryconf.h factory.h \
#		$(addprefix templates/,$(templatesrc)) \
#		$(addprefix templates/,$(templateincl))
#		$(CXX) -c $< $(libfactory_a_CXXFLAGS)  $(INCLUDES) -o $@

####################################################
## Test program
##
TESTS = test-g test-r
check_PROGRAMS = $(TESTS)

TESTSSOURCES = test.cc 

test_g: libfactory_g.a

test_g_SOURCES = $(TESTSSOURCES)
test_g_CFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long -fomit-frame-pointer --no-rtti #ftmpl_inst.o
test_g_LDADD = -L${builddir} -lfactory_g $(GMP_LIBS) $(NTL_LIBS)

test_r: libfactory.a

test_r_SOURCES = $(TESTSSOURCES)
test_r_CFLAGS = -O3 -fomit-frame-pointer --no-rtti #ftmpl_inst.o
test_r_LDADD = -L${builddir} -lfactory $(GMP_LIBS) $(NTL_LIBS)
