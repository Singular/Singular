CXXTEMPLFLAGS	=  --no-exceptions ## -fno-implicit-templates


DEBUGCXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long

NODEBUGCFLAGS   = -O3 -w -fomit-frame-pointer
NODEBUGCXXFLAGS = -O3 -w -fomit-frame-pointer --no-rtti 
NODEBUGDEFS = -DNDEBUG -DOM_NDEBUG 

AM_CFLAGS	= ${PIPE}
AM_CXXFLAGS	= ${PIPE} ${CXXTEMPLFLAGS}

INCLUDES	= -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. -I${srcdir} -I${includedir} ${GMP_CFLAGS}
AM_LDFLAGS	= -L${top_builddir}/coeffs -L${top_builddir}/reporter -L${top_builddir}/resources -L${top_builddir}/../omalloc -L${builddir}

if ENABLE_P_PROCS_STATIC
  USE_P_PROCS_STATIC_CC = templates/p_Procs_Static.cc
  USE_P_PROCS_STATIC = -Dp_Procs_Static
else
  USE_P_PROCS_STATIC_CC =
  USE_P_PROCS_STATIC =
endif

if ENABLE_P_PROCS_DYNAMIC
  USE_P_PROCS_DYNAMIC_CC = templates/p_Procs_Dynamic.cc
else
  USE_P_PROCS_DYNAMIC_CC =
endif

prCopy.inc: prCopy.pl
	perl ${srcdir}/prCopy.pl >  prCopy.inc

lib_LIBRARIES=libpolys.a libpolys_g.a

libpolys_a_includedir=$(includedir)/libpolys/polys
libpolys_g_a_includedir=$(includedir)/libpolys/polys

SOURCES = \
	monomials/monomials.cc monomials/p_polys.cc monomials/ring.cc monomials/maps.cc \
	operations/pShallowCopyDelete.cc operations/p_Mult_q.cc operations/p_Mult_q.h \
	templates/p_Procs_Lib.cc templates/p_MemCmp.h templates/p_MemCopy.h templates/p_Numbers.h \
	templates/p_Procs_Set.h templates/p_Procs_Static.h templates/p_Procs_Dynamic.h \
	templates/p_Procs_Impl.h \
	nc/sca.cc nc/summator.cc nc/ncSAFormula.cc nc/ncSACache.cc nc/ncSAMult.cc \
	pDebug.cc polys0.cc prCopy.cc prCopyMacros.h \
	kbuckets.cc sbuckets.cc weight.cc weight0.c simpleideals.cc matpol.cc \
	sparsmat.cc \
        ${USE_P_PROCS_STATIC_CC} ${USE_P_PROCS_DYNAMIC_CC}  mod_raw.cc \
        ext_fields/algext.cc ext_fields/algext.h \
	ext_fields/transext.cc clapsing.cc clapconv.cc \
	nc/old.gring.cc

#	OLD?: 

BUILT_SOURCES = prCopy.inc templates/p_Procs.inc

noinst_HEADERS =

LIBPOLYSHEADERS = monomials/ring.h monomials/monomials.h \
	nc/nc.h nc/sca.h nc/summator.h nc/ncSAFormula.h nc/ncSACache.h nc/ncSAMult.h \
	operations/pShallowCopyDelete.h \
	templates/p_MemAdd.h \
	templates/p_Procs.h mod_raw.h \
	monomials/p_polys.h monomials/maps.h prCopy.h \
	kbuckets.h sbuckets.h simpleideals.h weight.h matpol.h \
	sparsmat.h clapsing.h clapconv.h

### nobase_include_HEADERS = $(LIBPOLYSHEADERS)



P_PROCS_CPPFLAGS_COMMON = -DDYNAMIC_VERSION 

p_Procs_FieldGeneral_la_CPPFLAGS = -Dp_Procs_FieldGeneral ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGDEFS}
p_Procs_FieldIndep_la_CPPFLAGS = -Dp_Procs_FieldIndep ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGDEFS}
p_Procs_FieldQ_la_CPPFLAGS = -Dp_Procs_FieldQ ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGDEFS}
p_Procs_FieldZp_la_CPPFLAGS = -Dp_Procs_FieldZp ${P_PROCS_CPPFLAGS_COMMON} ${NODEBUGDEFS}

p_Procs_FieldGeneral_g_la_CPPFLAGS = -Dp_Procs_FieldGeneral ${P_PROCS_CPPFLAGS_COMMON}
p_Procs_FieldIndep_g_la_CPPFLAGS = -Dp_Procs_FieldIndep ${P_PROCS_CPPFLAGS_COMMON}
p_Procs_FieldQ_g_la_CPPFLAGS = -Dp_Procs_FieldQ ${P_PROCS_CPPFLAGS_COMMON}
p_Procs_FieldZp_g_la_CPPFLAGS = -Dp_Procs_FieldZp ${P_PROCS_CPPFLAGS_COMMON}

p_Procs_FieldGeneral_la_CXXFLAGS = ${NODEBUGCXXFLAGS}
p_Procs_FieldIndep_la_CXXFLAGS = ${NODEBUGCXXFLAGS}
p_Procs_FieldQ_la_CXXFLAGS = ${NODEBUGCXXFLAGS}
p_Procs_FieldZp_la_CXXFLAGS = ${NODEBUGCXXFLAGS}

p_Procs_FieldGeneral_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}
p_Procs_FieldIndep_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}
p_Procs_FieldQ_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}
p_Procs_FieldZp_g_la_CXXFLAGS = ${DEBUGCXXFLAGS}

P_PROCS_MODULE_LDFLAGS = -module 
# -L${top_builddir}/coeffs -lcoeffs 
P_PROCS_MODULE_LDFLAGSG = -module 
# -L${top_builddir}/coeffs -lcoeffs_g

p_Procs_FieldGeneral_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}
p_Procs_FieldIndep_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}
p_Procs_FieldQ_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}
p_Procs_FieldZp_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGS}

p_Procs_FieldGeneral_g_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGSG}
p_Procs_FieldIndep_g_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGSG}
p_Procs_FieldQ_g_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGSG}
p_Procs_FieldZp_g_la_LDFLAGS = ${P_PROCS_MODULE_LDFLAGSG}


P_PROCS = templates/p_Procs_Lib.cc

p_Procs_FieldGeneral_la_SOURCES = ${P_PROCS}
p_Procs_FieldIndep_la_SOURCES = ${P_PROCS}
p_Procs_FieldQ_la_SOURCES = ${P_PROCS}
p_Procs_FieldZp_la_SOURCES = ${P_PROCS}

p_Procs_FieldGeneral_g_la_SOURCES = ${P_PROCS}
p_Procs_FieldIndep_g_la_SOURCES = ${P_PROCS}
p_Procs_FieldQ_g_la_SOURCES = ${P_PROCS}
p_Procs_FieldZp_g_la_SOURCES = ${P_PROCS}


moduledir = $(libdir)/libpolys/MOD/

if ENABLE_P_PROCS_DYNAMIC
  module_LTLIBRARIES=p_Procs_FieldGeneral.la p_Procs_FieldIndep.la p_Procs_FieldQ.la p_Procs_FieldZp.la \
                     p_Procs_FieldGeneral_g.la p_Procs_FieldIndep_g.la p_Procs_FieldQ_g.la p_Procs_FieldZp_g.la
endif


nobase_libpolys_a_include_HEADERS = $(LIBPOLYSHEADERS)
nobase_libpolys_g_a_include_HEADERS = $(LIBPOLYSHEADERS)


libpolys_a_SOURCES = ${SOURCES}
libpolys_g_a_SOURCES = ${SOURCES}

## libpolys_adir = $(libdir)/libpolys
## libpolys_g_adir = $(libdir)/libpolys

EXTRA_libpolys_a_SOURCES = templates/p_Procs_Static.cc templates/p_Procs_Dynamic.cc

EXTRA_PROGRAMS = templates/p_Procs_Generate
templates_p_Procs_Generate_SOURCES = templates/p_Procs_Generate.cc


###test_LDADD   = libcoeffs.a -lresources -lreporter -lgmp -lomalloc

templates_p_Procs_Generate_CFLAGS   = ${NODEBUGCFLAGS}
templates_p_Procs_Generate_CXXFLAGS = ${NODEBUGCXXFLAGS}
templates_p_Procs_Generate_CPPFLAGS = ${NODEBUGDEFS} ${USE_P_PROCS_STATIC}
templates_p_Procs_Generate_LDADD    = -lreporter


templates/p_Procs.inc: templates/p_Procs_Generate
	${builddir}/templates/p_Procs_Generate > ${builddir}/templates/p_Procs.inc

libpolys_g_a_CXXFLAGS = ${DEBUGCXXFLAGS}
libpolys_a_CPPFLAGS = ${NODEBUGDEFS}

#  -fdiagnostics-show-option

dynamic_modules: ${module_LTLIBRARIES}

