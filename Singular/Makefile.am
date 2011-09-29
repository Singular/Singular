# PIPE = -pipe
# ???
#LD_DYN_FLAGS	= -ldl -rdynamic
#SFLAGS		= -fpic -DPIC
#SLDFLAGS	= -shared
#LD_LIBC	        = -lc
#STATIC_LDFLAGS	= -static


NODEBUGCFLAGS   = -O3 -w -fomit-frame-pointer
NODEBUGCXXFLAGS = -O3 -w -fomit-frame-pointer --no-rtti 
NODEBUGDEFS = -DNDEBUG -DOM_NDEBUG 

CXXTEMPLFLAGS = --no-exceptions ## -fno-implicit-templates

INCLUDES = -I${top_srcdir} -I${top_builddir} -I${top_srcdir}/numeric -I${top_builddir}/numeric -I${top_srcdir}/kernel -I${top_builddir}/kernel -I${top_srcdir}/libpolys -I${top_builddir}/libpolys $(GMP_CFLAGS) $(FACTORY_CFLAGS) $(NTL_CFLAGS)

########################### libSingular* #########################

libSingular_LIBRARIES = libSingular.a libSingular_g.a
libSingulardir = $(libdir)/singular

libSingular_a_CFLAGS   = -O3 -fomit-frame-pointer -Wextra -Wall -Wno-long-long ${PIPE}
libSingular_g_a_CFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE}

libSingular_a_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
libSingular_g_a_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)
# -fdiagnostics-show-option 

libSingular_a_CPPFLAGS   = -DNDEBUG -DOM_NDEBUG -DLIBSINGULAR
libSingular_g_a_CPPFLAGS = -DLIBSINGULAR

SOURCES =    Minor.cc\
   MinorInterface.cc\
   MinorProcessor.cc\
   attrib.cc\
   bbcone.cc\
   bbfan.cc\
   bigintm.cc\
   blackbox.cc\
   calcSVD.cc\
   cntrlc.cc\
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
   mpsr_Error.cc\
   mpsr_Get.cc\
   mpsr_GetMisc.cc\
   mpsr_GetPoly.cc\
   mpsr_Put.cc\
   mpsr_PutPoly.cc\
   mpsr_Timer.cc\
   mpsr_Tok.cc\
   mpsr_sl.cc\
   ndbm.cc\
   newstruct.cc\
   pcv.cc\
   pipeLink.cc\
   scanner.cc\
   sdb.cc\
   silink.cc\
   sing_dbm.cc\
   sing_win.cc\
   slInit_Dynamic.cc\
   slInit_Static.cc\
   ssiLink.cc\
   subexpr.cc\
   utils.cc\
   walk.cc\
   walk_ip.cc\
   wrapper.cc\
   fereadl.c mmstd.c claptmpl.cc


libSingular_a_SOURCES   = $(SOURCES)
libSingular_g_a_SOURCES = $(SOURCES)

SingularHEADERS =    Cache.h \
   CacheImplementation.h \
   Minor.h \
   MinorInterface.h \
   MinorProcessor.h \
   attrib.h \
   bbcone.h \
   bigintm.h \
   blackbox.h \
   cntrlc.h \
   dbm_sl.h \
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
   omSingularConfig.h \
   mpsr.h \
   mpsr_Get.h \
   mpsr_Put.h \
   mpsr_Timer.h \
   mpsr_Tok.h \
   mpsr_sl.h \
   ndbm.h \
   newstruct.h \
   pcv.h \
   pipeLink.h \
   pyobject_setup.h \
   run.h \
   sdb.h \
   silink.h \
   sing_dbm.h \
   sing_win.h \
   slInit.h \
   ssiLink.h \
   static.h \
   stype.h \
   subexpr.h \
   table.h \
   tok.h \
   utils.h \
   version.h \
   walk.h \
   feOpt.h feOpt.inc \
   mpsr_Tok.inc iparith.inc feOptTS.inc feOptES.inc

libSingular_includedir = ${includedir}/singular/Singular/
nobase_libSingular_include_HEADERS = $(SingularHEADERS)

if ENABLE_FACTORY
  USE_FACTORY = -L${top_builddir}/factory
else
  USE_FACTORY =
endif

# noinst_HEADERS = feOpt.h
## TODO: install all the *.inc?!  
## If not, move feOpt.h to SOURCES and remove *inc from HEADERS

#########################################################
# handle all the generated *.inc

AM_LDFLAGS = -L${top_builddir}/Singular -L${top_builddir}/numeric -L${top_builddir}/kernel -L${top_builddir}/libpolys/polys -L${top_builddir}/libpolys/coeffs -L${top_builddir}/libpolys/reporter -L${top_builddir}/libpolys/resources -L${top_builddir}/libpolys/misc $(USE_FACTORY) -L${top_builddir}/omalloc

noinst_PROGRAMS = gentable2 gentable1 feOpt feOptES feOptTS

gentable2_SOURCES = gentable2.cc grammar.h tok.h table.h
# gentable2_LDADD = -lSingular -lnumeric -lkernel -lpolys -lcoeffs -lresources -lreporter -lmisc $(FACTORY_LIBS) -lomalloc $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}
gentable2_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
gentable2_CXXFLAGS = -O3 -fomit-frame-pointer --no-rtti -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
gentable2_CPPFLAGS = -DNDEBUG -DOM_NDEBUG


gentable1_SOURCES = gentable.cc grammar.h tok.h table.h
# gentable1_LDADD = -lSingular -lnumeric -lkernel -lpolys -lcoeffs -lresources -lreporter -lmisc $(FACTORY_LIBS) -lomalloc $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}
gentable1_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
gentable1_CXXFLAGS = -O3 -fomit-frame-pointer --no-rtti -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
gentable1_CPPFLAGS = -DNDEBUG -DOM_NDEBUG

feOpt_SOURCES = feOptGen.cc feOptGen.h
feOpt_CXXFLAGS = -O3 -fomit-frame-pointer --no-rtti -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
feOpt_CPPFLAGS = -DNDEBUG -DOM_NDEBUG

feOptES_SOURCES = feOptGen.cc feOptGen.h
feOptES_CXXFLAGS = -O3 -fomit-frame-pointer --no-rtti -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
feOptES_CPPFLAGS = -DNDEBUG -DOM_NDEBUG -DESINGULAR 

feOptTS_SOURCES = feOptGen.cc feOptGen.h
feOptTS_CXXFLAGS = -O3 -fomit-frame-pointer --no-rtti -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
feOptTS_CPPFLAGS = -DNDEBUG -DOM_NDEBUG -DTSINGULAR

########################### Singular* #########################

bin_PROGRAMS = Singular Singularg

Singular_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
Singularg_CFLAGS = -g ${PIPE}

Singular_CXXFLAGS = -O3 -fomit-frame-pointer --no-rtti -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
Singularg_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)

Singular_SOURCES = tesths.cc
Singularg_SOURCES = tesths.cc

Singular_LDADD = libSingular.a -lnumeric -lkernel -lpolys -lcoeffs -lresources -lreporter -lmisc $(FACTORY_LIBS) -lomalloc $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}
Singularg_LDADD = libSingular_g.a -lnumeric_g -lkernel_g -lpolys_g -lcoeffs_g -lresources_g -lreporter_g -lmisc_g $(FACTORY_LIBS) -lomalloc_g $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS}  ${USEPPROCSDYNAMICLD}

Singular_CPPFLAGS = -DNDEBUG -DOM_NDEBUG
# Singularg_CPPFLAGS =


#########################################################
# the Singular library (*.lib files)

include ${srcdir}/singular-libs

singularlibrarydir = ${datadir}/singular
nobase_dist_singularlibrary_DATA = LIB/all.lib $(SLIB0:%=LIB/%) $(SLIB1:%=LIB/%) $(PLIBS:%=LIB/%)
EXTRA_DIST = make_alllib.sh LIB/all.lib.tmpl singular-libs

LIB/all.lib: ${srcdir}/make_alllib.sh ${srcdir}/LIB/all.lib.tmpl ${nobase_dist_singularlibrary_DATA}
	$(MKDIR_P) LIB
	${srcdir}/make_alllib.sh ${srcdir}/LIB/all.lib.tmpl ${SLIB0} ${PLIBS}

########################### TEST #########################

TESTS_ENVIRONMENT = SINGULARPATH='${top_builddir}/libpolys/tests'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${top_builddir}' 

TESTS=testg testr
check_PROGRAMS=$(TESTS)

testr_CFLAGS = -O3 -fomit-frame-pointer ${PIPE}
testg_CFLAGS = ${PIPE}

testr_CXXFLAGS = -O3 -fomit-frame-pointer --no-rtti -Wextra -Wall -Wno-long-long ${PIPE} $(CXXTEMPLFLAGS)
testg_CXXFLAGS = -g -Wextra -Wall -Wno-long-long -pedantic ${PIPE} $(CXXTEMPLFLAGS)

testr_SOURCES = test.cc
testg_SOURCES = test.cc

testr_LDADD = libSingular.a -lnumeric -lkernel -lpolys -lcoeffs -lresources -lreporter -lmisc $(FACTORY_LIBS) -lomalloc $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS} ${USEPPROCSDYNAMICLD}
testg_LDADD = libSingular_g.a -lnumeric_g -lkernel_g -lpolys_g -lcoeffs_g -lresources_g -lreporter_g -lmisc_g $(FACTORY_LIBS) -lomalloc_g $(NTL_LIBS) $(GMP_LIBS) ${USEPPROCSDYNAMICLDFLAGS}  ${USEPPROCSDYNAMICLD}

testr_CPPFLAGS = -DNDEBUG -DOM_NDEBUG
# testg_CPPFLAGS =

#########################################################

# These files are built first
BUILT_SOURCES = feOpt.inc mpsr_Tok.inc iparith.inc plural_cmd.inc feOptTS.inc feOptES.inc LIB/all.lib

CLEANFILES = $(BUILT_SOURCES)

feOpt.inc: feOpt
	./feOpt

feOptES.inc: feOptES
	./feOptES

feOptTS.inc: feOptTS
	./feOptTS

plural_cmd.inc iparith.inc: gentable1
	./gentable1

# fake rule to fix parallel make http://www.gnu.org/s/hello/manual/automake/Multiple-Outputs.html
plural_cmd.inc: iparith.inc

mpsr_Tok.inc: gentable2
	./gentable2

