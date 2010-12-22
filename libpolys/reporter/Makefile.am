lib_LIBRARIES=libreporter.a libreporter_g.a

AM_CFLAGS	= -O3 -w -fomit-frame-pointer ${PIPE}
CXXTEMPLFLAGS	= -fno-implicit-templates --no-exceptions
AM_CXXFLAGS	= -O3 -w -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
INCLUDES	= -I${top_srcdir}/.. -I${top_srcdir}/../.. -I${top_builddir}/..
AM_CPPFLAGS	= -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H

SOURCES = dError.c reporter.cc

libreporter_a_SOURCES = $(SOURCES)

libreporter_a_includedir=$(includedir)/libpolys/reporter

libreporter_a_include_HEADERS = reporter.h

libreporter_g_a_SOURCES = $(SOURCES)

libpolys_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -fdiagnostics-show-option -Wno-long-long
