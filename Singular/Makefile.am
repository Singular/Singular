ACLOCAL_AMFLAGS = -I ../m4

# PIPE = -pipe
# ???
#LD_DYN_FLAGS	= -ldl -rdynamic
#SFLAGS		= -fpic -DPIC
#SLDFLAGS	= -shared
#LD_LIBC	        = -lc
#STATIC_LDFLAGS	= -static

# WARNCFLAGS = -Wunused-parameter
# clang has also: -Wunneeded-internal-declaration

# NODEBUGCFLAGS   = -O3 -w -fomit-frame-pointer $(WARNCFLAGS)
# NODEBUGCXXFLAGS = -O3 -w -fomit-frame-pointer $(WARNCFLAGS)
# NODEBUGDEFS = -DNDEBUG -DOM_NDEBUG
# 


if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/factory
else
  USE_FACTORY =
endif

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/numeric -I${top_builddir}/numeric -I${top_srcdir}/kernel -I${top_builddir}/kernel -I${top_srcdir}/libpolys -I${top_builddir}/libpolys $(GMP_CFLAGS) -I${top_srcdir}/factory/include -I${top_builddir}/factory/include $(FACTORY_CFLAGS) $(NTL_CFLAGS) $(FLINT_CFLAGS) -DHAVE_CONFIG_H
AMLDFLAGS = -L${top_builddir}/Singular -L${top_builddir}/numeric -L${top_builddir}/kernel -L${top_builddir}/libpolys/polys  -L${top_builddir}/libpolys/coeffs -L${top_builddir}/libpolys/reporter -L${top_builddir}/libpolys/misc $(USE_FACTORY) -L${top_builddir}/omalloc -L${top_builddir}/resources

########################### Dynamic Modules... #########################
if ENABLE_P_PROCS_DYNAMIC
  moduledir = $(libexecdir)/singular/MOD
else !ENABLE_P_PROCS_DYNAMIC
  moduledir = $(libdir)/singular
endif !ENABLE_P_PROCS_DYNAMIC

if PYTHON_USE
  PY=pyobject.la
else !PYTHON_USE
  PY=
endif !PYTHON_USE

if SI_EMBED_PYTHON
  EMBED_PYOBJECT = ${PY} ${top_builddir}/dyn_modules/syzextra/syzextra.la 
else !SI_EMBED_PYTHON
  EMBED_PYOBJECT = 
endif !SI_EMBED_PYTHON

##########################################################################
module_LTLIBRARIES = pyobject.la

pyobject_la_SOURCES = pyobject.cc

# pyobject_la_CPPFLAGS   = ${AM_CPPFLAGS} ${NODEBUGDEFS}
# pyobject_g_la_CPPFLAGS = ${AM_CPPFLAGS}

pyobject_la_CXXFLAGS = @PYTHON_CSPEC@
pyobject_la_CFLAGS   = @PYTHON_CSPEC@ 

# ${NODEBUGCXXFLAGS} 
# pyobject_g_la_CXXFLAGS = ${DEBUGCXXFLAGS} @PYTHON_CSPEC@

pyobject_la_LDFLAGS = ${AMLDFLAGS} @PYTHON_LSPEC@ -module -export-dynamic -avoid-version -flat_namespace -weak_reference_mismatches weak -undefined dynamic_lookup
###  -export-dynamic -shared -module -avoid-version (add to all DMs?)


########################### libSingular* #########################

# if WANT_DEBUG
#   LIB_G=libSingular_g.la
# else
#   LIB_G=
# endif

libSingular_LTLIBRARIES = libSingular.la 
# ${LIB_G}
libSingulardir = $(libdir)/singular

# libSingular_la_CFLAGS   = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE}  $(WARNCFLAGS)
# libSingular_g_la_CFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE}  $(WARNCFLAGS)
# 
# libSingular_la_CXXFLAGS   = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)  $(WARNCFLAGS)
# libSingular_g_la_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)   $(WARNCFLAGS)
# # -fdiagnostics-show-option


# libSingular_la_CPPFLAGS   = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG -DLIBSINGULAR
# libSingular_g_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DLIBSINGULAR

libSingular_la_CPPFLAGS = ${AM_CPPFLAGS} -DLIBSINGULAR

SOURCES =    Minor.cc\
   MinorInterface.cc\
   MinorProcessor.cc\
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
   interpolation.cc\
   iparith.cc\
   ipassign.cc\
   ipconv.cc\
   ipid.cc\
   iplib.cc\
   ipprint.cc\
   ipshell.cc\
   janet.cc\
   libparse.cc\
   linearAlgebra_ip.cc\
   lists.cc\
   maps_ip.cc\
   minpoly.cc\
   misc_ip.cc\
   mod_lib.cc\
   links/ndbm.cc\
   newstruct.cc\
   pcv.cc\
   links/pipeLink.cc\
   scanner.cc\
   sdb.cc\
   links/s_buff.cc\
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
   fereadl.c mmstd.c claptmpl.cc

#   links/slInit_Dynamic.cc

libSingular_la_SOURCES   = $(SOURCES)
# libSingular_g_la_SOURCES = $(SOURCES)

SingularHEADERS =    Cache.h \
   CacheImplementation.h \
   Minor.h \
   MinorInterface.h \
   MinorProcessor.h \
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
   interpolation.h \
   idrec.h \
   ipconv.h \
   ipid.h \
   ipprint.h \
   ipshell.h \
   janet.h \
   libparse.h \
   libsingular.h \
   linearAlgebra_ip.h\
   lists.h \
   locals.h \
   maps_ip.h \
   minpoly.h \
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
   links/s_buff.h \
   links/sing_dbm.h \
   sing_win.h \
   links/slInit.h \
   links/ssiLink.h \
   links/simpleipc.h \
   si_signals.h \
   stype.h \
   subexpr.h \
   table.h \
   tok.h \
   walk.h


# -DMAKE_DISTRIBUTION 

# plural_cmd.inc
nodist_libSingular_la_SOURCES = feOpt.inc iparith.inc ${EMBED_PYOBJECT}
# nodist_libSingular_g_la_SOURCES = iparith.inc

libSingular_la_LDFLAGS    =  -release ${PACKAGE_VERSION} ${AMLDFLAGS} ${EMBED_PYOBJECT} \
-lnumeric -lkernel -lpolys -lcoeffs -lreporter -lmisc \
$(FACTORY_LIBS) -lomalloc -lresources $(FLINT_LIBS) $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}

libSingular_la_LIBADD      = ${EMBED_PYOBJECT} \
-lnumeric -lkernel -lpolys -lcoeffs -lreporter -lmisc \
$(FACTORY_LIBS) -lomalloc -lresources $(FLINT_LIBS) $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}

# ${top_builddir}/numeric/libnumeric.la ${top_builddir}/kernel/libkernel.la ${top_builddir}/libpolys/polys/libpolys.la 



libSingular_includedir = ${includedir}/singular/Singular/
nobase_libSingular_include_HEADERS = $(SingularHEADERS)

#########################################################
# handle all the generated *.inc

noinst_PROGRAMS = gentable1 feOptGen feOptES feOptTS

# CXXTEMPLFLAGS =-ftrapv ## -fno-implicit-templates

gentable1_SOURCES = gentable.cc grammar.h tok.h table.h
# gentable1_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
# gentable1_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
gentable1_CPPFLAGS = ${AM_CPPFLAGS}

feOptGen_SOURCES = feOptGen.cc feOptGen.h
# feOptGen_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
feOptGen_CPPFLAGS = ${AM_CPPFLAGS}

feOptES_SOURCES = feOptGen.cc feOptGen.h
# feOptES_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
feOptES_CPPFLAGS = ${AM_CPPFLAGS} -DESINGULAR

feOptTS_SOURCES = feOptGen.cc feOptGen.h
# feOptTS_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
feOptTS_CPPFLAGS = ${AM_CPPFLAGS} -DTSINGULAR

########################### Singular* #########################

EXTRA_PROGRAMS =  libparse Singulard

# the "optional_programs" variable should be defined in the configure
# script, and listed in an AC_SUBST macro
optional_Singular_programs = 
# Singularg 

bin_PROGRAMS = Singular ESingular TSingular $(optional_Singular_programs)

# if WANT_DEBUG
#   bin_PROGRAMS += Singularg
#   EXTRA_PROGRAMS += Singulardg
# endif

Singular ESingular TSingular $(optional_Singular_programs): ${abs_builddir}/LIB

# Singular_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
# Singularg_CFLAGS = -g ${PIPE}
# 
# Singular_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
# Singularg_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)

nodist_Singular_SOURCES = libSingular.la
# nodist_Singularg_SOURCES =  libSingular_g.la

Singular_SOURCES = tesths.cc mmalloc.cc fegetopt.c fegetopt.h utils.cc  utils.h
# Singularg_SOURCES = tesths.cc fegetopt.c fegetopt.h utils.cc  utils.h

Singular_LDADD = libSingular.la

# Singularg_LDADD = libSingular_g.la ${top_builddir}/numeric/libnumeric_g.la ${top_builddir}/kernel/libkernel_g.la ${top_builddir}/libpolys/polys/libpolys_g.la -lcoeffs_g -lreporter_g -lmisc_g $(FACTORY_LIBS) -lomalloc_g -lresources_g $(FLINT_LIBS) $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS}  ${USEPPROCSDYNAMICLD} ${EMBED_PYOBJECT_LDFLAGS}

# Singular_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
# Singularg_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H
# Singular_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H

Singular_LDFLAGS = -static ${AMLDFLAGS}
# Singularg_LDFLAGS = -static ${AMLDFLAGS}


# Singulard_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
# Singulardg_CFLAGS = -g ${PIPE}
# 
# Singulard_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
# Singulardg_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)

Singulard_SOURCES = tesths.cc mmalloc.cc fegetopt.c fegetopt.h utils.cc  utils.h
nodist_Singulard_SOURCES = libSingular.la

# Singulardg_SOURCES = tesths.cc fegetopt.c fegetopt.h utils.cc  utils.h
# nodist_Singulardg_SOURCES =  libSingular_g.la

#  ${EMBED_PYOBJECT_LDFLAGS}  ${EMBED_PYOBJECT_LDFLAGS}
Singulard_LDADD = libSingular.la

# Singulardg_LDADD = libSingular_g.la -lnumeric_g -lkernel_g -lpolys_g -lcoeffs_g -lreporter_g -lmisc_g $(FACTORY_LIBS) -lomalloc_g -lresources_g $(FLINT_LIBS) $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS}  ${USEPPROCSDYNAMICLD} ${EMBED_PYOBJECT_LDFLAGS}

# Singulard_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
# Singulardg_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H
# Singulard_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H

Singulard_LDFLAGS = ${AMLDFLAGS}
# Singulardg_LDFLAGS = ${AMLDFLAGS}


#### ESingular

# ESingular_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
# ESingular_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
#ESingular_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG -DESINGULAR -DPROTO
ESingular_CPPFLAGS = ${AM_CPPFLAGS} -DESINGULAR -DPROTO
ESingular_LDFLAGS = -static ${AMLDFLAGS}
ESingular_LDADD =   -lreporter -lmisc -lomalloc -lresources

ESingular_SOURCES = emacs.cc fegetopt.c fegetopt.h feOptES.inc feOpt.cc

#### same for TSingular

# TSingular_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
# TSingular_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
#TSingular_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG -DTSINGULAR -DPROTO
TSingular_CPPFLAGS = ${AM_CPPFLAGS} -DTSINGULAR -DPROTO
TSingular_LDFLAGS = -static ${AMLDFLAGS}
TSingular_LDADD =   -lreporter -lmisc -lomalloc -lresources

TSingular_SOURCES = emacs.cc fegetopt.c fegetopt.h feOptTS.inc feOpt.cc
# utils.cc utils.h


#### libparse
libparse_CPPFLAGS = ${AM_CPPFLAGS} -DSTANDALONE_PARSER
#libparse_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG -DSTANDALONE_PARSER
# libparse_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
# libparse_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)

libparse_SOURCES = libparse.cc fegetopt.c fegetopt.h utils.cc  utils.h
# nodist_libparse_SOURCES = libSingular.la

# # libSingular.la -lnumeric
libparse_LDADD = -lomalloc
libparse_LDFLAGS = -static ${AMLDFLAGS}
# TODO for libparse:
# multiple definition of `category_out': libparse.cc v.s. utils.cc
# undefined reference to `iiInitSingularProcinfo(procinfo*, char const*, char const*, int, long, int)'

#########################################################
# the Singular library (*.lib files)

include ${srcdir}/singular-libs

singularlibrarydir = ${datadir}/singular
singularlibrary = $(SLIB0:%=LIB/%) $(SLIB1:%=LIB/%) $(PLIBS:%=LIB/%)
nobase_dist_singularlibrary_DATA = LIB/tst.lib LIB/help.cnf ${singularlibrary}
EXTRA_DIST = make_alllib.sh LIB/all.lib.tmpl singular-libs \
             LIB/help.cnf links/slInit_Dynamic.cc

# ${nobase_dist_singularlibrary_DATA}

singularalllibrarydir = ${datadir}/singular/LIB
singularalllibrary = all.lib
nobase_dist_singularalllibrary_DATA = all.lib

all.lib: ${srcdir}/make_alllib.sh ${srcdir}/LIB/all.lib.tmpl ${singularlibrary}
	${srcdir}/make_alllib.sh ${srcdir}/LIB/all.lib.tmpl ${SLIB0} ${PLIBS}

########################### TEST #########################

TESTS_ENVIRONMENT = SINGULARPATH='${top_builddir}/Singular/LIB:${top_srcdir}/Singular/LIB:${top_builddir}/libpolys/polys/.libs:${top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${top_builddir}'

TESTS=test-s-r test-d-r

# if WANT_DEBUG
# TESTS += test-s-g test-d-g
# endif

check_PROGRAMS=$(TESTS)

# Static versions

# test_s_r_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
# test_s_g_CFLAGS = ${PIPE}
# 
# test_s_r_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
# test_s_g_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)

test_s_r_SOURCES = test.cc
#test_s_g_SOURCES = test.cc
# ${EMBED_PYOBJECT_LDFLAGS}
test_s_r_LDADD = libSingular.la 

#-lnumeric -lkernel -lpolys -lcoeffs \
#-lreporter -lmisc $(FACTORY_LIBS) -lomalloc -lresources $(FLINT_LIBS) \
#$(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD} \
#${EMBED_PYOBJECT}

#test_s_g_LDADD = libSingular_g.la 
#-lnumeric_g -lkernel_g -lpolys_g \
#-lcoeffs_g -lreporter_g -lmisc_g $(FACTORY_LIBS) -lomalloc_g \
#-lresources_g $(FLINT_LIBS) $(NTL_LIBS) $(GMP_LIBS) \
#${USEPPROCSDYNAMICLDFLAGS}  ${USEPPROCSDYNAMICLD} \
#${EMBED_PYOBJECT}


# test_s_r_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H
#test_s_r_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
#test_s_g_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H

test_s_r_LDFLAGS = -static ${AMLDFLAGS}
#test_s_g_LDFLAGS = -static ${AMLDFLAGS}


# Dynamic versions

# test_d_r_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
# test_d_g_CFLAGS = ${PIPE}
# 
# test_d_r_CXXFLAGS = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
# test_d_g_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)

test_d_r_SOURCES = test.cc
# test_d_g_SOURCES = test.cc
#  ${EMBED_PYOBJECT_LDFLAGS}
test_d_r_LDADD = libSingular.la 
# -lnumeric -lkernel -lpolys -lcoeffs -lreporter -lmisc $(FACTORY_LIBS) -lomalloc -lresources $(FLINT_LIBS) $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}
# test_d_g_LDADD = libSingular_g.la -lnumeric_g -lkernel_g -lpolys_g -lcoeffs_g -lreporter_g -lmisc_g $(FACTORY_LIBS) -lomalloc_g -lresources_g $(FLINT_LIBS) $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS}  ${USEPPROCSDYNAMICLD} ${EMBED_PYOBJECT_LDFLAGS}

#test_d_r_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
# test_d_r_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H
# test_d_g_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H

#test_d_r_LDFLAGS = ${AMLDFLAGS}
# test_d_g_LDFLAGS = ${AMLDFLAGS}

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

