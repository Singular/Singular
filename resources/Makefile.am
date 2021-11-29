ACLOCAL_AMFLAGS = -I ../m4

libsingular_resources_LTLIBRARIES = libsingular_resources.la

libsingular_resourcesdir = $(libdir)


AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir}


SOURCES  = omFindExec.c feResource.cc feFopen.cc fegetopt.c
libsingular_resources_la_SOURCES   = $(SOURCES)

libsingular_resources_la_LDFLAGS   = $(SINGULAR_LDFLAGS) -release ${PACKAGE_VERSION}

libsingular_resources_includedir  =$(includedir)/resources

libsingular_resources_include_HEADERS = omFindExec.h feResource.h feFopen.h fegetopt.h

nodist_libsingular_resources_include_HEADERS = singular_resourcesconfig.h
DISTCLEANFILES = $(nodist_libsingular_resources_include_HEADERS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = singular_resources.pc
