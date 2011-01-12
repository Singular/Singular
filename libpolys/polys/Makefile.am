CXXTEMPLFLAGS	= -fno-implicit-templates --no-exceptions
AM_CFLAGS	= -O3 -w -fomit-frame-pointer ${PIPE}
AM_CXXFLAGS	= -O3 -w -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
INCLUDES	= -I${top_srcdir}/.. -I${top_srcdir}/../.. -I${top_builddir}/.. -I${srcdir} -I${includedir}
AM_CPPFLAGS	= -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H
AM_LDFLAGS	= -L${top_builddir}/../coeffs -L${top_builddir}/../reporter -L${top_builddir}/../resources -L${top_builddir}/../../omalloc -L${builddir}

## we removed -Dix86_Linux from AM_CPPFLAGS

SOURCES = \
	monomials/monomials.cc monomials/p_polys.cc monomials/ring.cc \
	pDebug.cc pInline0.cc pInline1.cc pInline2.cc \
	operations/pShallowCopyDelete.cc operations/p_Mult_q.cc \
	p_Procs_Lib.cc polys-impl.cc polys.cc \
	polys0.cc polys1.cc prCopy.cc prCopyTemplate.cc

noinst_HEADERS =

include_HEADERS = \
	monomials/ring.h \
	pInline0.h pInline1.h pInline2.h operations/pShallowCopyDelete.h \
	p_MemAdd.h p_MemCmp.h p_MemCopy.h operations/p_Mult_q.h p_Numbers.h p_Procs.h \
	p_Procs_Dynamic.h p_Procs_Impl.h p_Procs_Set.h p_Procs_Static.h \
	monomials/p_polys.h monomials/polys-impl.h monomials/polys.h prCopy.h prCopyMacros.h

lib_LIBRARIES=libpolys.a libpolys_g.a

libpolys_a_SOURCES = ${SOURCES}
libpolys_g_a_SOURCES = ${SOURCES}

EXTRA_libpolys_a_SOURCES = p_Procs_Dynamic.cc p_Procs_Static.cc

libpolys_a_LIBADD = $(DL_KERNEL_OBJECT)
libpolys_a_DEPENDENCIES = $(DL_KERNEL_OBJECT)

libpolys_g_a_LIBADD = $(DL_KERNEL_OBJECT_DEBUG)
libpolys_g_a_DEPENDENCIES = $(DL_KERNEL_OBJECT_DEBUG)

EXTRA_PROGRAMS = p_Procs_Generate
p_Procs_Generate_SOURCES = p_Procs_Generate.cc

p_Procs.inc: p_Procs_Generate
	${builddir}/p_Procs_Generate > ${builddir}/p_Procs.inc

libpolys_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -fdiagnostics-show-option -Wno-long-long
