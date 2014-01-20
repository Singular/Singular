ACLOCAL_AMFLAGS = -I ../m4

libresources_LTLIBRARIES = libresources.la

libresourcesdir = $(libdir)


AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} 


SOURCES  = omFindExec.c feResource.cc feFopen.cc
libresources_la_SOURCES   = $(SOURCES)

libresources_la_LDFLAGS   = -release ${PACKAGE_VERSION}

libresources_includedir  =$(includedir)/resources

libresources_include_HEADERS = omFindExec.h feResource.h feFopen.h

nodist_libresources_include_HEADERS = resourcesconfig.h
DISTCLEANFILES = $(nodist_libresources_include_HEADERS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = resources.pc
