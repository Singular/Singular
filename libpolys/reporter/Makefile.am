libreporter_LTLIBRARIES=libreporter.la libreporter_g.la
libreporterdir = $(libdir)/singular

CXXTEMPLFLAGS =  --no-exceptions ## -fno-implicit-templates

libreporter_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libreporter_g_la_CFLAGS = ${PIPE}

libreporter_la_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
libreporter_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

libreporter_la_CPPFLAGS   = -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H
libreporter_g_la_CPPFLAGS = -DHAVE_CONFIG_H

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..

SOURCES  = dError.c reporter.cc
libreporter_la_SOURCES   = $(SOURCES)
libreporter_g_la_SOURCES = $(SOURCES)

libreporter_includedir  =$(includedir)/singular/reporter

libreporter_include_HEADERS   = reporter.h 



