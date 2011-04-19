ACLOCAL_AMFLAGS = -I m4 -I ../../m4
CXXTEMPLFLAGS	= -fno-implicit-templates --no-exceptions


DEBUGCXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long

NODEBUGCFLAGS   = -O3 -w -fomit-frame-pointer
NODEBUGCXXFLAGS = -O3 -w -fomit-frame-pointer --no-rtti 
NODEBUGCPPFLAGS = -DNDEBUG -DOM_NDEBUG 

AM_CPPFLAGS	= -DHAVE_CONFIG_H

AM_CFLAGS	= ${PIPE}
AM_CXXFLAGS	= ${PIPE} ${CXXTEMPLFLAGS}

INCLUDES	= -I${top_srcdir}/.. -I${top_srcdir}/../.. -I${top_builddir}/.. -I${srcdir} -I${includedir} ${GMP_CFLAGS}
AM_LDFLAGS	= -L${top_builddir}/../coeffs -L${top_builddir}/../reporter -L${top_builddir}/../resources -L${top_builddir}/../../omalloc -L${builddir}

if ENABLE_P_PROCS_STATIC
  USE_P_PROCS_STATIC_CC = templates/p_Procs_Static.cc
else
  USE_P_PROCS_STATIC_CC =
endif

if ENABLE_P_PROCS_DYNAMIC
  USE_P_PROCS_DYNAMIC_CC = templates/p_Procs_Dynamic.cc
else
  USE_P_PROCS_DYNAMIC_CC =
endif

SOURCES = \
	monomials/monomials.cc monomials/p_polys.cc monomials/ring.cc monomials/maps.cc \
	operations/pShallowCopyDelete.cc operations/p_Mult_q.cc \
	templates/p_Procs_Lib.cc \
	nc/gring.cc nc/sca.cc nc/summator.cc nc/ncSAFormula.cc nc/ncSACache.cc nc/ncSAMult.cc \
	pDebug.cc pInline0.cc polys0.cc prCopy.cc \
	kbuckets.cc sbuckets.cc ${USE_P_PROCS_STATIC_CC} ${USE_P_PROCS_DYNAMIC_CC} weight.cc \
        weight0.c simpleideals.cc matpol.cc

BUILT_SOURCES = templates/p_Procs.inc

noinst_HEADERS =

include_HEADERS = \
	monomials/ring.h nc/nc.h \
  nc/sca.h nc/summator.h nc/ncSAFormula.h nc/ncSACache.h nc/ncSAMult.h \
	pInline0.h operations/pShallowCopyDelete.h \
	templates/p_MemAdd.h templates/p_MemCmp.h templates/p_MemCopy.h operations/p_Mult_q.h \
	templates/p_Numbers.h templates/p_Procs.h \
	templates/p_Procs_Dynamic.h templates/p_Procs_Impl.h templates/p_Procs_Set.h templates/p_Procs_Static.h \
	monomials/p_polys.h monomials/polys-impl.h monomials/maps.h polys.h prCopy.h prCopyMacros.h \
	kbuckets.h sbuckets.h simpleideals.h weight.h \
  matpol.h

P_PROCS_CPPFLAGS_COMMON = -DHAVE_CONFIG_H -DDYNAMIC_VERSION 

libp_Procs_FieldGeneral_la_CPPFLAGS = -Dp_Procs_FieldGeneral ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGCPPFLAGS}
libp_Procs_FieldIndep_la_CPPFLAGS = -Dp_Procs_FieldIndep ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGCPPFLAGS}
libp_Procs_FieldQ_la_CPPFLAGS = -Dp_Procs_FieldQ ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGCPPFLAGS}
libp_Procs_FieldZp_la_CPPFLAGS = -Dp_Procs_FieldZp ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGCPPFLAGS}

libp_Procs_FieldGeneral_g_la_CPPFLAGS = -Dp_Procs_FieldGeneral ${P_PROCS_CPPFLAGS_COMMON}
libp_Procs_FieldIndep_g_la_CPPFLAGS = -Dp_Procs_FieldIndep ${P_PROCS_CPPFLAGS_COMMON}
libp_Procs_FieldQ_g_la_CPPFLAGS = -Dp_Procs_FieldQ ${P_PROCS_CPPFLAGS_COMMON}
libp_Procs_FieldZp_g_la_CPPFLAGS = -Dp_Procs_FieldZp ${P_PROCS_CPPFLAGS_COMMON}

libp_Procs_FieldGeneral_la_CXXFLAGS = ${NODEBUGCXXFLAGS}
libp_Procs_FieldIndep_la_CXXFLAGS = ${NODEBUGCXXFLAGS}
libp_Procs_FieldQ_la_CXXFLAGS = ${NODEBUGCXXFLAGS}
libp_Procs_FieldZp_la_CXXFLAGS = ${NODEBUGCXXFLAGS}

libp_Procs_FieldGeneral_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}
libp_Procs_FieldIndep_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}
libp_Procs_FieldQ_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}
libp_Procs_FieldZp_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}


P_PROCS = templates/p_Procs_Lib.cc

libp_Procs_FieldGeneral_la_SOURCES = ${P_PROCS}
libp_Procs_FieldIndep_la_SOURCES = ${P_PROCS}
libp_Procs_FieldQ_la_SOURCES = ${P_PROCS}
libp_Procs_FieldZp_la_SOURCES = ${P_PROCS}

libp_Procs_FieldGeneral_g_la_SOURCES = ${P_PROCS}
libp_Procs_FieldIndep_g_la_SOURCES = ${P_PROCS}
libp_Procs_FieldQ_g_la_SOURCES = ${P_PROCS}
libp_Procs_FieldZp_g_la_SOURCES = ${P_PROCS}



lib_LIBRARIES=libpolys.a libpolys_g.a

moduledir = ${libdir}/singular/MOD/

module_LTLIBRARIES=libp_Procs_FieldGeneral.la libp_Procs_FieldIndep.la libp_Procs_FieldQ.la libp_Procs_FieldZp.la \
                   libp_Procs_FieldGeneral_g.la libp_Procs_FieldIndep_g.la libp_Procs_FieldQ_g.la libp_Procs_FieldZp_g.la

libpolys_a_SOURCES = ${SOURCES}
libpolys_g_a_SOURCES = ${SOURCES}

EXTRA_libpolys_a_SOURCES = templates/p_Procs_Static.cc templates/p_Procs_Dynamic.cc

libpolys_a_LIBADD = $(DL_KERNEL_OBJECT)
libpolys_a_DEPENDENCIES = $(DL_KERNEL_OBJECT)

libpolys_g_a_LIBADD = $(DL_KERNEL_OBJECT_DEBUG)
libpolys_g_a_DEPENDENCIES = $(DL_KERNEL_OBJECT_DEBUG)


libpolys_a_includedir  =$(includedir)/libpolys/polys
libpolys_g_a_includedir=$(includedir)/libpolys/polys

EXTRA_PROGRAMS = templates/p_Procs_Generate
templates_p_Procs_Generate_SOURCES = templates/p_Procs_Generate.cc


###test_LDADD   = libcoeffs.a -lresources -lreporter -lgmp -lomalloc

templates_p_Procs_Generate_CFLAGS   = ${NODEBUGCFLAGS}
templates_p_Procs_Generate_CXXFLAGS = ${NODEBUGCXXFLAGS}
templates_p_Procs_Generate_CPPFLAGS = ${NODEBUGCPPFLAGS}
templates_p_Procs_Generate_LDADD    = -lreporter


templates/p_Procs.inc: templates/p_Procs_Generate
	${builddir}/templates/p_Procs_Generate > ${builddir}/templates/p_Procs.inc

libpolys_g_a_CXXFLAGS = ${DEBUGCXXFLAGS}
#  -fdiagnostics-show-option

dynamic_modules: ${module_LTLIBRARIES}

prCopy.inc: prCopy.pl
	perl prCopy.pl > prCopy.xx && mv prCopy.xx prCopy.inc

prCopy.o: prCopy.inc
