ACLOCAL_AMFLAGS = -I ../m4

SUBDIRS=include/factory

AM_CPPFLAGS = -I${builddir}/include -I${srcdir}/include -I${srcdir} \
$(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
$(FLINT_CFLAGS) $(NTL_CPPFLAGS) $(GMP_CPPFLAGS) $(SINGULAR_CFLAGS)

lib_LTLIBRARIES = libfactory.la

libfactory_la_LIBADD     =$(RESOURCES_LIBS) $(OMALLOC_LIBS) \
$(FLINT_LIBS) $(NTL_LIBS) $(GMP_LIBS)

libfactory_la_LDFLAGS    = $(SINGULAR_LDFLAGS) -release ${PACKAGE_VERSION}

# factory source files
SOURCES = \
		canonicalform.cc \
		cf_algorithm.cc \
		cf_char.cc \
		cfCharSets.cc \
		cfCharSetsUtil.cc \
		cf_chinese.cc \
		cf_cyclo.cc \
		cf_eval.cc \
		cfEzgcd.cc \
		cf_factor.cc \
		cf_factory.cc \
		cf_gcd.cc \
		cfGcdAlgExt.cc \
		cfGcdUtil.cc \
		cf_generator.cc \
		cf_globals.cc \
		cf_hnf.cc \
		cf_inline.cc \
		cf_irred.cc \
		cf_iter.cc \
		cf_iter_inline.cc \
		cf_linsys.cc \
		cf_map.cc \
		cf_map_ext.cc \
		cfModGcd.cc \
		cfNewtonPolygon.cc \
		cfNTLzzpEXGCD.cc \
		cfModResultant.cc \
		cf_ops.cc \
		cf_primes.cc \
		cf_random.cc \
		cf_resultant.cc \
		cf_reval.cc \
		cfSubResGcd.cc \
		cf_switches.cc \
		cf_util.cc \
		cfUnivarGcd.cc \
		debug.cc \
		DegreePattern.cc \
		ExtensionInfo.cc \
		facAbsBiFact.cc \
		facAbsFact.cc \
		facAlgExt.cc \
		facAlgFunc.cc \
		facAlgFuncUtil.cc \
		facBivar.cc \
		facFactorize.cc \
		fac_sqrfree.cc \
		fac_util.cc \
		facFqBivar.cc \
		facFqBivarUtil.cc \
		facFqFactorize.cc \
		facFqFactorizeUtil.cc \
		facFqSquarefree.cc \
		facHensel.cc \
		facIrredTest.cc \
		facMul.cc \
		facSparseHensel.cc \
		ffops.cc \
		FLINTconvert.cc \
		gf_tabutil.cc \
		gfops.cc \
		imm.cc \
		int_cf.cc \
		int_int.cc \
		int_intdiv.cc \
		int_poly.cc \
		int_rat.cc \
		variable.cc \
		NTLconvert.cc \
		singext.cc \
		parseutil.cc \
		ftmpl_inst.cc

if WITH_PARSER_FOR_CANONICAL_FORM
    SOURCES +=  readcf.yy
endif

libfactory_la_SOURCES = $(SOURCES)

nodist_libfactory_la_SOURCES = factory.h factoryconf.h

# factory header files
factory_headers = \
		cf_assert.h \
		canonicalform.h \
		cf_algorithm.h \
		cfCharSets.h \
		cfCharSetsUtil.h \
		cf_cyclo.h \
		cf_defs.h \
		cf_eval.h \
		cfEzgcd.h \
		cf_factory.h \
		cf_generator.h \
		cf_globals.h \
		cfGcdAlgExt.h \
		cfGcdUtil.h \
		cf_hnf.h \
		cf_irred.h \
		cf_iter.h \
		cf_map.h \
		cf_map_ext.h \
		cfModGcd.h \
		cfNewtonPolygon.h \
		cfNTLzzpEXGCD.h \
		cfModResultant.h \
		cf_primes.h \
		cf_primetab.h \
		cf_random.h \
		cf_reval.h \
		cfSubResGcd.h \
		cf_switches.h \
		cf_util.h \
		cfUnivarGcd.h \
		debug.h \
		DegreePattern.h \
		ExtensionInfo.h \
		facAbsBiFact.h \
		facAbsFact.h \
		facAlgExt.h \
		facAlgFunc.h \
		facAlgFuncUtil.h \
		facBivar.h \
		facFactorize.h \
		fac_sqrfree.h \
		fac_util.h \
		facFqBivar.h \
		facFqBivarUtil.h \
		facFqFactorize.h \
		facFqFactorizeUtil.h \
		facFqSquarefree.h \
		facHensel.h \
		facIrredTest.h \
		facMul.h \
		facSparseHensel.h \
		ffops.h \
		FLINTconvert.h \
		gf_tabutil.h \
		gfops.h \
		gmpext.h \
		imm.h \
		int_cf.h \
		int_int.h \
		int_poly.h \
		int_rat.h \
		timing.h \
		variable.h \
		NTLconvert.h \
		singext.h \
		parseutil.h

noinst_HEADERS = $(factory_headers)

libfactory_includedir = ${includedir}/factory

nodist_libfactory_include_HEADERS = factory.h factoryconf.h
libfactory_include_HEADERS = globaldefs.h


####################################################
# Documentation
include $(srcdir)/aminclude.am

####################################################
# the precomputed GF(q)-tables

gftablesdir=$(datadir)/factory/gftables
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


####################################################
# Support for building GF(q)-tables
#
# gengftables needs libfactory.la and factory.h AND is needed for
# generating gftable but the tables are not necessarily needed to
# compile and use libfactory (though it will be a lot slower).

EXTRA_PROGRAMS = gengftables
gengftables_SOURCES = gengftables-conway.cc
gengftables_LDADD = libfactory.la $(FLINT_LIBS) $(GMP_LIBS) $(NTL_LIBS)


####################################################
# These files listed below are not used anywhere but are included in
# the distribution. So they will be tacked on to EXTRA_DIST.

templatesrc =	templates/ftmpl_array.cc \
		templates/ftmpl_afactor.cc \
		templates/ftmpl_factor.cc \
		templates/ftmpl_functions.h \
		templates/ftmpl_list.cc \
		templates/ftmpl_matrix.cc

# header templates
hdrtemplsrc = 	factoryconf.template \
		factory.template

EXTRA_DIST =	test_install.cc \
		$(templatesrc) $(hdrtemplsrc) \
		doxygen.cfg \
		examples/application.cc \
		examples/factorize.cc examples/gcd.cc \
		bin/folding.el bin/fold-docu.el \
		bin/makeheader bin/gen-readcf readcf.yy \
		make_factory_dist

##################################################
# autogenerated sources

BUILT_SOURCES = factory.h factoryconf.h

CLEANFILES = $(BUILT_SOURCES) include/factory/factory.h include/factory/factoryconf.h

DISTCLEANFILES =  config.h readcf.cc

#distclean-local:
#	-rm -f readcf.cc

.template.h: $(factory_headers)
	${srcdir}/bin/makeheader $< $@
	cp $@ include/factory/

# factory/gfops.cc has the gftables path hardcoded in, so we create a
# symlink. Remove this rule once gfopts.cc has been fixed. Note that
# check-local would run in parallel with check, so we can't use it
# here!
all-local:
	[ -d ${builddir}/gftables ] || ln -s ${srcdir}/gftables ${builddir}


####################################################
## Test program
##

TESTS = test

check_PROGRAMS = $(TESTS)

test_SOURCES = test.cc
test_LDADD   = libfactory.la $(libfactory_la_LIBADD)

####################################################
## PKG config
##

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = factory.pc

