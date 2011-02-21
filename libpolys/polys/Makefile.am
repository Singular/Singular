CXXTEMPLFLAGS	= -fno-implicit-templates --no-exceptions
AM_CFLAGS	= -O3 -w -fomit-frame-pointer ${PIPE}
AM_CXXFLAGS	= -O3 -w -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
INCLUDES	= -I${top_srcdir}/.. -I${top_srcdir}/../.. -I${top_builddir}/.. -I${srcdir} -I${includedir}
AM_CPPFLAGS	= -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H
AM_LDFLAGS	= -L${top_builddir}/../coeffs -L${top_builddir}/../reporter -L${top_builddir}/../resources -L${top_builddir}/../../omalloc -L${builddir}

## we removed -Dix86_Linux from AM_CPPFLAGS

SOURCES = \
	monomials/monomials.cc monomials/p_polys.cc monomials/ring.cc monomials/maps.cc \
	pDebug.cc pInline0.cc \
	operations/pShallowCopyDelete.cc operations/p_Mult_q.cc \
	templates/p_Procs_Lib.cc polys.cc \
	polys0.cc polys1.cc prCopy.cc prCopyTemplate.cc \
	sbuckets.cc templates/p_Procs.cc

BUILT_SOURCES = templates/p_Procs.inc

noinst_HEADERS =

include_HEADERS = \
	monomials/ring.h \
	pInline0.h operations/pShallowCopyDelete.h \
	templates/p_MemAdd.h templates/p_MemCmp.h templates/p_MemCopy.h operations/p_Mult_q.h \
	templates/p_Numbers.h templates/p_Procs.h \
	templates/p_Procs_Dynamic.h templates/p_Procs_Impl.h templates/p_Procs_Set.h templates/p_Procs_Static.h \
	monomials/p_polys.h monomials/polys-impl.h monomials/maps.h polys.h prCopy.h prCopyMacros.h \
	sbuckets.h simpleideals.h

lib_LIBRARIES=libpolys.a libpolys_g.a

libpolys_a_SOURCES = ${SOURCES}
libpolys_g_a_SOURCES = ${SOURCES}

libpolys_a_LIBADD = $(DL_KERNEL_OBJECT)
libpolys_a_DEPENDENCIES = $(DL_KERNEL_OBJECT)

libpolys_g_a_LIBADD = $(DL_KERNEL_OBJECT_DEBUG)
libpolys_g_a_DEPENDENCIES = $(DL_KERNEL_OBJECT_DEBUG)

EXTRA_PROGRAMS = templates/p_Procs_Generate
templates_p_Procs_Generate_SOURCES = templates/p_Procs_Generate.cc

templates/p_Procs.inc: templates/p_Procs_Generate
	${builddir}/templates/p_Procs_Generate > ${builddir}/templates/p_Procs.inc

libpolys_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long
#  -fdiagnostics-show-option
