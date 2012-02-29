noinst_LTLIBRARIES=libresources.la libresources_g.la
# libresourcesdir = $(libdir)/singular

CXXTEMPLFLAGS      =  ## -fno-implicit-templates

libresources_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libresources_g_la_CFLAGS = ${PIPE}

libresources_la_CXXFLAGS   = -O3 -fomit-frame-pointer ${PIPE} ${CXXTEMPLFLAGS}
libresources_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

libresources_la_CPPFLAGS   = -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
libresources_g_la_CPPFLAGS = -DHAVE_CONFIG_H

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..

SOURCES = feFopen.cc
libresources_la_SOURCES   = $(SOURCES)
libresources_g_la_SOURCES = $(SOURCES)

libresources_includedir  =$(includedir)/singular/resources

libresources_include_HEADERS   = feFopen.h



