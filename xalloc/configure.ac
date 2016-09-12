ACLOCAL_AMFLAGS = -I ../m4

lib_LTLIBRARIES=libomalloc.la

libomalloc_includedir=$(includedir)/omalloc

libomalloc_include_HEADERS = omalloc.h omallocClass.h

SOURCES=dummy.c omallocClass.cc

AM_CPPFLAGS =-I${top_srcdir}/.. -I${top_builddir}/..

libomalloc_la_SOURCES=$(SOURCES) $(noinst_HEADERS)

libomalloc_la_LDFLAGS    = -release ${PACKAGE_VERSION}

libomalloc_la_CPPFLAGS  = ${AM_CPPFLAGS}

omTables_CPPFLAGS = ${AM_CPPFLAGS}

CLEANFILES = $(BUILT_SOURCES)

prefix=@prefix@
exec_prefix=@exec_prefix@
libdir=@libdir@
includedir=@includedir@

Name: omalloc
Description: The Singular memory manager
Version: @PACKAGE_VERSION@
URL: https://github.com/Singular/Sources/tree/spielwiese/@PACKAGE@

# Requires:
# Conflicts:

Cflags: -I${includedir}
Libs: -L${libdir} -lomalloc
# Libs.private:


