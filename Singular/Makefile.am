ACLOCAL_AMFLAGS = -I ../m4

GIT_VERSION := $(shell $(top_srcdir)/git-version-gen $(top_srcdir)/.tarball-git-version)

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/kernel -I${top_builddir}/kernel -I${top_srcdir}/libpolys -I${top_builddir}/libpolys $(FACTORY_INCLUDES)  $(GMP_CFLAGS) $(NTL_CFLAGS) $(FLINT_CFLAGS) -DGIT_VERSION='"$(GIT_VERSION)"'

if HAVE_GFANLIB
  USE_GFANLIB = ${top_builddir}/dyn_modules/callgfanlib/gfanlib.la ${abs_top_builddir}/gfanlib/libgfan.la ${CDDGMPLDFLAGS}
else
  USE_GFANLIB = 
endif

# AM_LDFLAGS = libSingular.la ${top_builddir}/kernel/libkernel.la

########################### Dynamic Modules... #########################
if ENABLE_P_PROCS_DYNAMIC
  moduledir = $(libexecdir)/singular/MOD
else !ENABLE_P_PROCS_DYNAMIC
  moduledir = $(libdir)/singular
endif !ENABLE_P_PROCS_DYNAMIC

##########################################################################
module_LTLIBRARIES = pyobject.la

pyobject_la_SOURCES = pyobject.cc

pyobject_la_CXXFLAGS = $(PYTHON_CSPEC)
pyobject_la_CPPFLAGS = $(PYTHON_CPPFLAGS)  ${AM_CPPFLAGS}

pyobject_la_LDFLAGS = $(PYTHON_EXTRA_LIBS)  $(PYTHON_LSPEC) ${AM_LDFLAGS} $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_LDFLAGS) -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
###  -export-dynamic -shared -module -avoid-version (add to all DMs?)

##########################################################################
if PYTHON_USE
  PY=pyobject.la  $(PYTHON_EXTRA_LIBS)  $(PYTHON_LSPEC) $(PYTHON_EXTRA_LDFLAGS) $(PYTHON_LDFLAGS) 
else !PYTHON_USE
  PY=
endif !PYTHON_USE

if SI_EMBED_PYTHON
 EMBED_PYOBJECT   = ${PY} ${top_builddir}/dyn_modules/syzextra/syzextra.la ${USE_GFANLIB}
else !SI_EMBED_PYTHON
  EMBED_PYOBJECT = 
endif !SI_EMBED_PYTHON

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
   pcv.cc\
   links/pipeLink.cc\
   scanner.cc\
   sdb.cc\
   links/silink.cc\
   links/sing_dbm.cc\
   sing_win.cc\
   links/slInit_Static.cc\
   links/ssiLink.cc\
   links/semaphore.c\
   subexpr.cc\
   pyobject_setup.cc\
   singmathic.cc\
   walk.cc\
   walk_ip.cc\
   wrapper.cc\
   mmstd.c claptmpl.cc\
   febase.cc

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
   feOpt.h feOptTab.h fehelp.h \
   fglm.h \
   gms.h \
   grammar.h \
   idrec.h \
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
   omSingularConfig.h \
   links/ndbm.h \
   newstruct.h \
   pcv.h \
   links/pipeLink.h \
   pyobject_setup.h \
   run.h \
   sdb.h \
   links/silink.h \
   links/sing_dbm.h \
   sing_win.h \
   links/slInit.h \
   links/ssiLink.h \
   links/simpleipc.h \
   stype.h \
   subexpr.h \
   table.h \
   tok.h \
   walk.h \
   fevoices.h


# -DMAKE_DISTRIBUTION 

# plural_cmd.inc
nodist_libSingular_la_SOURCES = feOpt.inc iparith.inc 

libSingular_la_LDFLAGS    =${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD} -release ${PACKAGE_VERSION} ${PTHREAD_LDFLAGS}
libSingular_la_LIBADD     =${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD} ${EMBED_PYOBJECT} ${top_builddir}/kernel/libkernel.la ${PTHREAD_LIBS}

libSingular_includedir = ${includedir}/singular/Singular
nobase_libSingular_include_HEADERS = $(SingularHEADERS)

#########################################################
# handle all the generated *.inc

noinst_PROGRAMS = gentable1 feOptGen feOptES feOptTS

gentable1_SOURCES = gentable.cc grammar.h tok.h table.h
gentable1_CPPFLAGS = ${AM_CPPFLAGS}

feOptGen_SOURCES = feOptGen.cc feOptGen.h
feOptGen_CPPFLAGS = ${AM_CPPFLAGS}

feOptES_SOURCES = feOptGen.cc feOptGen.h
feOptES_CPPFLAGS = ${AM_CPPFLAGS} -DESINGULAR

feOptTS_SOURCES = feOptGen.cc feOptGen.h
feOptTS_CPPFLAGS = ${AM_CPPFLAGS} -DTSINGULAR

########################### Singular* #########################

EXTRA_PROGRAMS =  libparse Singulars

# the "optional_programs" variable should be defined in the configure
# script, and listed in an AC_SUBST macro
optional_Singular_programs = 

bin_PROGRAMS = Singular ESingular TSingular $(optional_Singular_programs)

Singular ESingular TSingular $(optional_Singular_programs): ${abs_builddir}/LIB

Singular_SOURCES = tesths.cc mmalloc.cc fegetopt.c fegetopt.h utils.cc  utils.h

Singular_LDADD = libSingular.la ${EMBED_PYOBJECT}

Singular_LDFLAGS = -static ${EMBED_PYOBJECT}

Singulars_SOURCES = tesths.cc mmalloc.cc fegetopt.c fegetopt.h utils.cc  utils.h

#  ${EMBED_PYOBJECT_LDFLAGS}  ${EMBED_PYOBJECT_LDFLAGS}
Singulars_LDADD = libSingular.la ${EMBED_PYOBJECT}

Singulars_LDFLAGS = -static ${AM_LDFLAGS} ${EMBED_PYOBJECT}

scriptdir = $(libexecdir)/singular/MOD
dist_script_SCRIPTS = singularsurf

#### ESingular
ESingular_CPPFLAGS = ${AM_CPPFLAGS} -DESINGULAR -DPROTO
# ESingular_LDFLAGS = -static ${AM_LDFLAGS}
ESingular_LDADD =  ${top_builddir}/libpolys/reporter/libreporter.la \
${top_builddir}/libpolys/misc/libmisc.la ${top_builddir}/omalloc/libomalloc.la \
${top_builddir}/resources/libresources.la


ESingular_SOURCES = emacs.cc fegetopt.c fegetopt.h feOptES.inc feOpt.cc

#### same for TSingular
TSingular_CPPFLAGS = ${AM_CPPFLAGS} -DTSINGULAR -DPROTO
# TSingular_LDFLAGS = -static ${AM_LDFLAGS}
TSingular_LDADD = ${top_builddir}/libpolys/reporter/libreporter.la \
${top_builddir}/libpolys/misc/libmisc.la ${top_builddir}/omalloc/libomalloc.la \
${top_builddir}/resources/libresources.la

TSingular_SOURCES = emacs.cc fegetopt.c fegetopt.h feOptTS.inc feOpt.cc
# utils.cc utils.h


#### libparse
libparse_CPPFLAGS = ${AM_CPPFLAGS} -DSTANDALONE_PARSER

libparse_SOURCES = libparse.cc fegetopt.c fegetopt.h utils.cc  utils.h

libparse_LDADD = ${top_builddir}/omalloc/libomalloc.la
libparse_LDFLAGS = -static ${AM_LDFLAGS}

#########################################################
# the Singular library (*.lib files)

include ${srcdir}/singular-libs

singularlibrarydir = ${datadir}/singular
singularlibrary = $(SLIB0:%=LIB/%) $(SLIB1:%=LIB/%) $(PLIBS:%=LIB/%)
nobase_dist_singularlibrary_DATA = LIB/tst.lib LIB/help.cnf ${singularlibrary}
EXTRA_DIST = make_alllib.sh LIB/all.lib.tmpl singular-libs \
             LIB/help.cnf links/slInit_Dynamic.cc

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
# ${EMBED_PYOBJECT_LDFLAGS}
test_LDADD = libSingular.la 

#########################################################

# These files are built first
BUILT_SOURCES = feOpt.inc feOptTS.inc feOptES.inc iparith.inc all.lib plural_cmd.inc \
                gftables MOD

CLEANFILES = $(BUILT_SOURCES) $(EXTRA_PROGRAMS)

clean-local:
	if [ "${abs_builddir}" != "${abs_srcdir}" ]; then \
	  if [ -L ${abs_builddir}/LIB ]; then unlink ${abs_builddir}/LIB; fi \
	fi

feOpt.inc: feOptGen$(EXEEXT)
	${builddir}/feOptGen$(EXEEXT)

feOptES.inc: feOptES$(EXEEXT)
	${builddir}/feOptES$(EXEEXT)

feOptTS.inc: feOptTS$(EXEEXT)
	${builddir}/feOptTS$(EXEEXT)

plural_cmd.inc iparith.inc: gentable1$(EXEEXT)
	${builddir}/gentable1$(EXEEXT)

# # fake rule to fix parallel make http://www.gnu.org/s/hello/manual/automake/Multiple-Outputs.html
# plural_cmd.inc: iparith.inc

gftables: ${top_srcdir}/factory/gftables
	ln -snf ${top_srcdir}/factory/gftables ${builddir}/gftables

${abs_builddir}/LIB: ${srcdir}/LIB
	if [ "${abs_builddir}" != "${abs_srcdir}" ]; then \
	  ln -snf ${abs_srcdir}/LIB ${abs_builddir}/; \
	fi

MOD: ${top_builddir}/libpolys/polys/.libs
	ln -snf ${top_builddir}/libpolys/polys/.libs/ ${builddir}/MOD

