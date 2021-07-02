ACLOCAL_AMFLAGS = -I../m4

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/kernel -I${top_builddir}/kernel -I${top_srcdir}/libpolys -I${top_builddir}/libpolys $(FACTORY_INCLUDES) $(CCLUSTER_CPPFLAGS)  $(GMP_CPPFLAGS) $(FLINT_CFLAGS)

AM_CXXFLAGS=$(NTL_CPPFLAGS) -fno-delete-null-pointer-checks

all-local: all.lib gftables MOD ${abs_builddir}/LIB Singular$(EXEEXT) ESingular$(EXEEXT) TSingular$(EXEEXT)

if SING_WIN
SUBDIRS = ${BUILTIN_MODULES} . dyn_modules
else
SUBDIRS = dyn_modules .
endif

########################### Possible builtin modules... #########################
BUILTIN_FLAGS = ${BUILTIN_LIBS}

########################### libSingular* #########################

libSingular_LTLIBRARIES = libSingular.la
libSingulardir = $(libdir)

libSingular_la_CPPFLAGS = ${AM_CPPFLAGS} -DLIBSINGULAR ${PTHREAD_CFLAGS}

SOURCES = \
   links/asciiLink.cc\
   attrib.cc\
   blackbox.cc\
   calcSVD.cc\
   cntrlc.cc\
   countedref.cc\
   denom_list.cc\
   eigenval_ip.cc\
   extra.cc\
   feOpt.cc\
   fehelp.cc\
   fevoices.cc\
   fglm.cc\
   gms.cc\
   grammar.cc\
   iparith.cc\
   ipassign.cc\
   ipconv.cc\
   ipid.cc\
   iplib.cc\
   ipprint.cc\
   ipshell.cc\
   libparse.cc\
   linearAlgebra_ip.cc\
   lists.cc\
   maps_ip.cc\
   misc_ip.cc\
   mod_lib.cc\
   links/ndbm.cc\
   newstruct.cc\
   number2.cc\
   pcv.cc\
   links/pipeLink.cc\
   scanner.cc\
   sdb.cc\
   links/silink.cc\
   links/sing_dbm.cc\
   links/slInit_Static.cc\
   links/ssiLink.cc\
   links/semaphore.c\
   subexpr.cc\
   pyobject_setup.cc\
   walk.cc\
   walk_ip.cc\
   wrapper.cc\
   mmstd.c claptmpl.cc\
   febase.cc \
   svd/libs/amp.cpp \
   svd/libs/ap.cpp

#   links/slInit_Dynamic.cc

libSingular_la_SOURCES   = $(SOURCES)

SingularHEADERS = \
   attrib.h \
   blackbox.h \
   cntrlc.h \
   countedref.h \
   links/dbm_sl.h \
   distrib.h \
   eigenval_ip.h \
   feOpt.h feOptTab.h fehelp.h feOpt.inc feOptTS.inc feOptES.inc\
   fglm.h \
   gms.h \
   grammar.h \
   idrec.h \
   iparith.inc \
   ipconv.h \
   ipid.h \
   ipprint.h \
   ipshell.h \
   libparse.h \
   libsingular.h \
   linearAlgebra_ip.h\
   lists.h \
   locals.h \
   maps_ip.h \
   misc_ip.h \
   mmalloc.h \
   mod_lib.h \
   links/ndbm.h \
   newstruct.h \
   number2.h \
   pcv.h \
   links/pipeLink.h \
   pyobject_setup.h \
   sdb.h \
   links/silink.h \
   links/sing_dbm.h \
   links/slInit.h \
   links/ssiLink.h \
   links/simpleipc.h \
   stype.h \
   subexpr.h \
   table.h \
   tok.h \
   walk.h \
   fevoices.h \
   svd_si.h \
   svd/libs/amp.h  svd/libs/ap.h  svd/libs/stdafx.h \
   svd/bdsvd.h  svd/bidiagonal.h  svd/blas.h  svd/lq.h  svd/qr.h  svd/reflections.h \
   svd/rotations.h  svd/svd.h



# -DMAKE_DISTRIBUTION

# plural_cmd.inc
nodist_libSingular_la_SOURCES = feOpt.inc iparith.inc

libSingular_la_LDFLAGS    =$(SINGULAR_LDFLAGS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD} -release ${PACKAGE_VERSION} $(CCLUSTER_LIBS) ${PTHREAD_LDFLAGS}
libSingular_la_LIBADD     =${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD} ${BUILTIN_FLAGS} ${top_builddir}/kernel/libkernel.la ${PTHREAD_LIBS}

libSingular_includedir = ${includedir}/singular/Singular
nobase_libSingular_include_HEADERS = $(SingularHEADERS)

########################### Singular* #########################
dist_man1_MANS = Singular.man ESingular.man TSingular.man

EXTRA_PROGRAMS =  libparse Singulard

# the "optional_Singular_programs" variable should be defined in the configure
# script, and listed in an AC_SUBST macro

bin_PROGRAMS = Singular ESingular TSingular $(optional_Singular_programs)

Singular_SOURCES = tesths.cc fegetopt.c fegetopt.h utils.cc  utils.h

Singular_LDADD = libSingular.la ${OMALLOC_LIBS} ${BUILTIN_FLAGS}

Singular_LDFLAGS = ${AM_LDFLAGS} ${BUILTIN_FLAGS}

Singulard_SOURCES = tesths.cc fegetopt.c fegetopt.h utils.cc  utils.h

Singulard_LDADD = libSingular.la ${BUILTIN_FLAGS}

Singulard_LDFLAGS = -shared ${AM_LDFLAGS} ${BUILTIN_FLAGS}

scriptdir = $(libexecdir)/singular/MOD
dist_script_SCRIPTS = singularsurf singularsurf_jupyter singularsurf_win surfex

#### ESingular
ESingular_CPPFLAGS = ${AM_CPPFLAGS} -DESINGULAR -DPROTO
ESingular_LDADD =  ${top_builddir}/libpolys/reporter/libreporter.la \
${top_builddir}/libpolys/misc/libmisc.la ${OMALLOC_LIBS} \
${top_builddir}/resources/libsingular_resources.la


ESingular_SOURCES = emacs.cc fegetopt.c fegetopt.h feOptES.inc feOpt.cc

#### same for TSingular
TSingular_CPPFLAGS = ${AM_CPPFLAGS} -DTSINGULAR -DPROTO
TSingular_LDADD = ${top_builddir}/libpolys/reporter/libreporter.la \
${top_builddir}/libpolys/misc/libmisc.la ${OMALLOC_LIBS} \
${top_builddir}/resources/libsingular_resources.la

TSingular_SOURCES = emacs.cc fegetopt.c fegetopt.h feOptTS.inc feOpt.cc
# utils.cc utils.h


#### libparse
libparse_CPPFLAGS = ${AM_CPPFLAGS} -DSTANDALONE_PARSER

libparse_SOURCES = libparse.cc fegetopt.c fegetopt.h utils.cc  utils.h

libparse_LDADD =
libparse_LDFLAGS = ${AM_LDFLAGS}

#########################################################
# the Singular library (*.lib files)

include ${srcdir}/singular-libs

singularlibrarydir = ${datadir}/singular
singularlibrary = $(SLIB0:%=LIB/%) $(SLIB1:%=LIB/%) $(PLIBS:%=LIB/%) $(SLIB2:%=LIB/%)
nobase_dist_singularlibrary_DATA = LIB/tst.lib LIB/help.cnf ${singularlibrary}
EXTRA_DIST = grammar.y libparse.ll make_alllib.sh make_grammar make_table \
             LIB/all.lib.tmpl singular-libs LIB/help.cnf LIB/.singularrc \
	     LIB/COPYING links/slInit_Dynamic.cc gentable.cc

singularalllibrarydir = ${datadir}/singular/LIB
singularalllibrary = all.lib
nobase_dist_singularalllibrary_DATA = all.lib

all.lib: ${srcdir}/make_alllib.sh ${srcdir}/LIB/all.lib.tmpl ${singularlibrary}
	${srcdir}/make_alllib.sh ${srcdir}/LIB/all.lib.tmpl ${SLIB0} ${PLIBS}

########################### TEST #########################
TESTS_ENVIRONMENT = SINGULARPATH='${top_builddir}/Singular/LIB:${top_srcdir}/Singular/LIB:${top_builddir}/libpolys/polys/.libs:${top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${top_builddir}'

TESTS=test

check_PROGRAMS=$(TESTS)

test_SOURCES = test.cc
test_LDADD = libSingular.la

#########################################################
# These files are built first
BUILT_SOURCES = all.lib gftables MOD
CLEANFILES = $(BUILT_SOURCES) $(EXTRA_PROGRAMS)

clean-local:
	if [ "${abs_builddir}" != "${abs_srcdir}" ]; then \
	  if [ -L ${abs_builddir}/LIB ]; then unlink ${abs_builddir}/LIB; fi \
	fi

# # fake rule to fix parallel make http://www.gnu.org/s/hello/manual/automake/Multiple-Outputs.html

gftables: ${top_srcdir}/factory/gftables
	ln -snf ${top_srcdir}/factory/gftables ${builddir}/gftables

${abs_builddir}/LIB: ${srcdir}/LIB
	if [ "${abs_builddir}" != "${abs_srcdir}" ]; then \
	  ln -snf ${abs_srcdir}/LIB ${abs_builddir}/; \
	fi

MOD:
	if [ -d "${top_builddir}/libpolys/polys/.libs" ]; then \
	  ln -snf ${top_builddir}/libpolys/polys/.libs/ ${abs_builddir}/MOD; \
	fi

echoLIBS:
	@echo ${SLIB0} ${SLIB1} ${SLIB2} ${PLIBS}

