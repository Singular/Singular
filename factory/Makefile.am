CXXTEMPLFLAGS      = -fno-implicit-templates --no-exceptions

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${srcdir} ${GMP_CFLAGS} ${NTL_CFLAGS}

lib_LIBRARIES = libfactory.a libfactory_g.a

libfactory_a_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${CXXTEMPLFLAGS}
libfactory_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${CXXTEMPLFLAGS}

# factory source files
SOURCES = \
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
		fac_berlekamp.cc \
		fac_cantzass.cc \
		fac_distrib.cc \
		fac_ezgcd.cc \
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
		readcf.y \
		ftmpl_inst.cc

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
		fac_berlekamp.h \
		fac_cantzass.h \
		fac_distrib.h \
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

noinst_PROGRAMS = gengftables
gengftables_LDADD = -L${builddir} -lfactory $(GMP_LIBS) $(NTL_LIBS)

gengftables_SOURCES = gengftables-conway.cc ftmpl_inst.cc

gftables:	gengftables
		@echo "This command will take quite a long time to execute;"
		@echo "maybe you want to get the tables directly via ftp (see"
		@echo "the INSTALL file)."
		$(MKINSTALLDIRS) gftables
		./gengftables

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

EXTRA_DIST =	gengftables.cc gengftables-conway.cc test_install.cc \
		$(templatesrc) $(templateincl) $(hdrtemplsrc) \
		$(ftestsrc) $(ftestincl) $(ftestm4src) $(ftestdistfiles) \
		doxygen.cfg factory.cfg gen_cf_gmp.template \
		examples/application.cc \
		examples/factorize.cc examples/gcd.cc \
		bin/folding.el bin/fold-docu.el \
		bin/makeheader bin/gen-readcf
