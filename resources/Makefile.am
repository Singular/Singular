ACLOCAL_AMFLAGS = -I ../m4

#if WANT_DEBUG
#  LIB_G=libresources_g.la
#else
#  LIB_G=
#endif

libresources_LTLIBRARIES = libresources.la
# ${LIB_G}
#libresourcesdir = $(libdir)/

#CXXTEMPLFLAGS =-ftrapv 
### -fno-implicit-templates

#libresources_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
#libresources_g_la_CFLAGS = ${PIPE}

#libresources_la_CXXFLAGS   = -O3 -fomit-frame-pointer ${PIPE} ${CXXTEMPLFLAGS} -fno-exceptions -fno-rtti
#libresources_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir} -DHAVE_CONFIG_H

# libresources_la_CPPFLAGS   += ${AM_CPPFLAGS}

#libresources_la_CPPFLAGS   = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
#libresources_g_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H 

SOURCES  = omFindExec.c feResource.cc feFopen.cc
libresources_la_SOURCES   = $(SOURCES)
# libresources_g_la_SOURCES = $(SOURCES)

### libresources_la_LDFLAGS    = -release ${PACKAGE_VERSION}

libresources_includedir  =$(includedir)/resources

libresources_include_HEADERS = omFindExec.h feResource.h feFopen.h

nodist_libresources_include_HEADERS = resourcesconfig.h


