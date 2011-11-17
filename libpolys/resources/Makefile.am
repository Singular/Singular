noinst_LTLIBRARIES=libresources.la libresources_g.la
# libresourcesdir = $(libdir)/singular

CXXTEMPLFLAGS      =  --no-exceptions ## -fno-implicit-templates

libresources_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libresources_g_la_CFLAGS = ${PIPE}

libresources_la_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
libresources_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

libresources_la_CPPFLAGS   = -DNDEBUG -DOM_NDEBUG
#libresources_g_la_CPPFLAGS =

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..

SOURCES = feFopen.cc feResource.cc
libresources_la_SOURCES   = $(SOURCES)
libresources_g_la_SOURCES = $(SOURCES)

libresources_includedir  =$(includedir)/singular/resources

libresources_include_HEADERS   = feFopen.h feResource.h



