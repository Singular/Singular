ACLOCAL_AMFLAGS = -I ../m4

bin_SCRIPTS = libpolys-config

SUBDIRS=tests

configheaderdir = ${includedir}/singular
nodist_configheader_HEADERS = libpolysconfig.h
DISTCLEANFILES = $(nodist_configheader_HEADERS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libpolys.pc





AM_CPPFLAGS	= -I${top_srcdir} -I${top_builddir} \
${FACTORY_INCLUDES} ${RESOURCES_INCLUDES} ${OMALLOC_INCLUDES} \
${FLINT_CFLAGS} ${GMP_CPPFLAGS}

AM_CXXFLAGS =


if ENABLE_P_PROCS_STATIC
  USE_P_PROCS_STATIC_CC = polys/templates/p_Procs_Static.cc
  USE_P_PROCS_STATIC = -Dp_Procs_Static
else
  USE_P_PROCS_STATIC_CC =
  USE_P_PROCS_STATIC =
endif

if ENABLE_P_PROCS_DYNAMIC
  USE_P_PROCS_DYNAMIC_CC = polys/templates/p_Procs_Dynamic.cc
  P_PROCS_CPPFLAGS_COMMON = ${AM_CPPFLAGS} -DDYNAMIC_VERSION
else
  USE_P_PROCS_DYNAMIC_CC =
  P_PROCS_CPPFLAGS_COMMON = ${AM_CPPFLAGS}
endif


lib_LTLIBRARIES = libpolys.la
libpolysdir = $(libdir)

SOURCES = \
        coeffs/numbers.cc coeffs/rintegers.cc coeffs/rmodulo2m.cc coeffs/rmodulon.cc coeffs/shortfl.cc \
        coeffs/gnumpc.cc coeffs/gnumpfl.cc coeffs/longrat.cc coeffs/longrat0.cc coeffs/ffields.cc \
        coeffs/modulop.cc coeffs/mpr_complex.cc coeffs/ntupel.cc \
        coeffs/bigintmat.cc  \
        coeffs/flintcf_Qrat.cc coeffs/flintcf_Q.cc coeffs/flintcf_Zn.cc \
	polys/monomials/p_polys.cc polys/monomials/ring.cc polys/monomials/maps.cc \
	polys/operations/pShallowCopyDelete.cc polys/operations/p_Mult_q.cc \
	polys/nc/sca.cc polys/nc/summator.cc polys/nc/ncSAFormula.cc polys/nc/ncSACache.cc polys/nc/ncSAMult.cc \
	polys/pDebug.cc polys/polys0.cc polys/prCopy.cc polys/prCopyMacros.h \
	polys/kbuckets.cc polys/sbuckets.cc polys/weight.cc polys/weight0.c polys/simpleideals.cc polys/matpol.cc \
	polys/sparsmat.cc \
	polys/shiftop.cc \
        ${USE_P_PROCS_STATIC_CC} ${USE_P_PROCS_DYNAMIC_CC} polys/mod_raw.cc \
        polys/ext_fields/algext.cc polys/ext_fields/transext.cc \
	polys/clapsing.cc polys/clapconv.cc  polys/flintconv.cc polys/flint_mpoly.cc\
	polys/nc/old.gring.cc polys/PolyEnumerator.cc \
        misc/int64vec.cc misc/options.c misc/sirandom.c misc/intvec.cc misc/prime.cc \
        reporter/dError.cc reporter/reporter.cc reporter/s_buff.cc

LIBPOLYSHEADERS = polys/monomials/ring.h polys/monomials/monomials.h \
        polys/monomials/p_polys.h polys/monomials/maps.h polys/PolyEnumerator.h  polys/prCopy.h \
	polys/nc/nc.h polys/nc/sca.h polys/nc/summator.h polys/nc/ncSAFormula.h polys/nc/ncSACache.h polys/nc/ncSAMult.h polys/nc/gb_hack.h \
	polys/operations/pShallowCopyDelete.h \
	polys/templates/p_Procs.h polys/templates/p_MemAdd.h polys/templates/p_MemCmp.h \
	polys/kbuckets.h polys/sbuckets.h polys/simpleideals.h polys/weight.h polys/matpol.h \
	polys/sparsmat.h polys/clapsing.h polys/clapconv.h polys/coeffrings.h polys/flintconv.h polys/flint_mpoly.h\
	polys/shiftop.h \
	polys/ext_fields/algext.h polys/ext_fields/transext.h polys/mod_raw.h \
        misc/mylimits.h misc/options.h misc/intvec.h misc/int64vec.h \
        misc/sirandom.h misc/prime.h misc/auxiliary.h \
        reporter/reporter.h reporter/s_buff.h reporter/si_signals.h \
        coeffs/coeffs.h coeffs/numbers.h coeffs/si_gmp.h coeffs/gnumpc.h coeffs/gnumpfl.h coeffs/longrat.h coeffs/modulop.h coeffs/modulop_inl.h \
        coeffs/ffields.h coeffs/rintegers.h coeffs/rmodulo2m.h coeffs/rmodulon.h \
        coeffs/shortfl.h coeffs/mpr_complex.h coeffs/mpr_global.h coeffs/numstats.h coeffs/ntupel.h \
        coeffs/bigintmat.h coeffs/Enumerator.h \
        coeffs/flintcf_Qrat.h coeffs/flintcf_Q.h coeffs/flintcf_Zn.h

libpolys_la_includedir  =$(includedir)/singular
nobase_libpolys_la_include_HEADERS=${LIBPOLYSHEADERS}

EXTRA_DIST = \
	polys/prCopy.pl polys/prCopyTemplate.cc \
	polys/templates/p_Procs_Lib.cc polys/templates/p_MemCopy.h polys/templates/p_Numbers.h \
	polys/templates/p_Procs_Set.h polys/templates/p_Procs_Static.h polys/templates/p_Procs_Dynamic.h \
	polys/templates/p_Procs_Impl.h polys/templates/p_Delete__T.cc polys/templates/p_Copy__T.cc \
	polys/templates/p_ShallowCopyDelete__T.cc polys/templates/p_Mult_nn__T.cc \
	polys/templates/pp_Mult_nn__T.cc polys/templates/pp_Mult_mm__T.cc polys/templates/p_Mult_mm__T.cc \
	polys/templates/pp_Mult_Coeff_mm_DivSelect__T.cc polys/templates/pp_Mult_nn__T.cc \
	polys/templates/pp_Mult_Coeff_mm_DivSelectMult__T.cc \
	polys/templates/pp_Mult_mm_Noether__T.cc polys/operations/p_Mult_q.h \
	polys/templates/p_kBucketSetLm__T.cc polys/templates/p_Minus_mm_Mult_qq__T.cc \
	polys/templates/p_Merge_q__T.cc polys/templates/p_Add_q__T.cc polys/templates/p_Neg__T.cc \
	coeffs/rintegers2.cc coeffs/rintegers3.cc


p_Procs_FieldGeneral_la_CPPFLAGS = -Dp_Procs_FieldGeneral ${P_PROCS_CPPFLAGS_COMMON}
p_Procs_FieldIndep_la_CPPFLAGS = -Dp_Procs_FieldIndep ${P_PROCS_CPPFLAGS_COMMON}
p_Procs_FieldQ_la_CPPFLAGS = -Dp_Procs_FieldQ ${P_PROCS_CPPFLAGS_COMMON}
p_Procs_FieldZp_la_CPPFLAGS = -Dp_Procs_FieldZp ${P_PROCS_CPPFLAGS_COMMON}

P_PROCS_MODULE_LDFLAGS = -shared -module -dynamic -export-dynamic -avoid-version -weak_reference_mismatches weak -undefined dynamic_lookup -flat_namespace

p_Procs_FieldGeneral_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}
p_Procs_FieldIndep_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}
p_Procs_FieldQ_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}
p_Procs_FieldZp_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}

P_PROCS = polys/templates/p_Procs_Lib.cc

p_Procs_FieldGeneral_la_SOURCES = ${P_PROCS}
p_Procs_FieldIndep_la_SOURCES = ${P_PROCS}
p_Procs_FieldQ_la_SOURCES = ${P_PROCS}
p_Procs_FieldZp_la_SOURCES = ${P_PROCS}


moduledir = $(libexecdir)/singular/MOD

if ENABLE_P_PROCS_DYNAMIC
  module_LTLIBRARIES=p_Procs_FieldGeneral.la p_Procs_FieldIndep.la p_Procs_FieldQ.la p_Procs_FieldZp.la
endif

libpolys_includedir=$(includedir)/singular/polys

nobase_libpolys_include_HEADERS = $(LIBPOLYSHEADERS)

libpolys_la_SOURCES = ${SOURCES}

EXTRA_libpolys_la_SOURCES = polys/templates/p_Procs_Static.cc polys/templates/p_Procs_Dynamic.cc

libpolys_la_LDFLAGS= -no-undefined $(SINGULAR_LDFLAGS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD} -release ${PACKAGE_VERSION}
libpolys_la_LIBADD = ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD} ${FACTORY_LIBS}


#################################################
# autogenerated sources
noinst_PROGRAMS = p_Procs_Generate
nodist_p_Procs_Generate_SOURCES = polys/templates/p_Procs.inc
p_Procs_Generate_SOURCES = polys/p_Procs_Generate.cc
p_Procs_Generate_CPPFLAGS = ${AM_CPPFLAGS} ${USE_P_PROCS_STATIC}
BUILT_SOURCES = polys/prCopy.inc gftables MOD polys/templates/p_Procs.inc

polys/templates/p_Procs.inc: polys/p_Procs_Generate.cc
	$(MAKE) $(AM_MAKEFLAGS) CXX="$(CXX_FOR_BUILD)" \
	CXXFLAGS="$(CXXFLAGS_FOR_BUILD)" \
	CPPFLAGS="-I${abs_top_srcdir} -I${abs_top_builddir} -I${top_srcdir} -I${top_builddir} ${FACTORY_INCLUDES} $(CPPFLAGS_FOR_BUILD) ${USE_P_PROCS_STATIC}" \
	LDFLAGS="" LIBS="" p_Procs_Generate${BUILD_EXEEXT}
	./p_Procs_Generate$(BUILD_EXEEXT) > ./polys/templates/p_Procs.inc

polys/prCopy.inc: polys/prCopy.pl
	perl ${srcdir}/polys/prCopy.pl >  polys/prCopy.inc

CLEANFILES = $(BUILT_SOURCES) p_Procs_Generate${BUILD_EXEEXT}

######################################################################

TESTS = test1 test2
check_PROGRAMS = $(TESTS)

test1_LDADD=libpolys.la \
$(FACTORY_LIBS) $(RESOURCES_LIBS) $(OMALLOC_LIBS) \
$(FLINT_LIBS) $(GMP_LIBS) $(NTL_LIBS)

test1_SOURCES=coeffs/test.cc

test2_LDADD=libpolys.la \
$(FACTORY_LIBS) $(RESOURCES_LIBS) $(OMALLOC_LIBS) \
$(FLINT_LIBS) $(GMP_LIBS) $(NTL_LIBS)

test2_SOURCES=polys/test.cc

gftables: ${top_srcdir}/../factory/gftables
	ln -snf ${top_srcdir}/../factory/gftables ${builddir}/gftables

MOD:
	if [ -d "${top_builddir}/libpolys/.libs" ]; then \
	  ln -snf ${top_builddir}/libpolys/.libs/ ${abs_builddir}/MOD; \
	fi

