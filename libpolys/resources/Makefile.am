lib_LIBRARIES=libresources.a libresources_g.a

AM_CFLAGS	= -O3 -w -fomit-frame-pointer ${PIPE}
CXXTEMPLFLAGS	= -fno-implicit-templates --no-exceptions
AM_CXXFLAGS	= -O3 -w -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
INCLUDES	= -I${top_srcdir}/.. -I${top_srcdir}/../.. -I${top_builddir}/..
AM_CPPFLAGS	= -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H

SOURCES = feFopen.cc feResource.cc

libresources_a_SOURCES = $(SOURCES)

libresources_a_includedir=$(includedir)/libpolys/resources

libresources_a_include_HEADERS = feFopen.h feResource.h

libresources_g_a_SOURCES = $(SOURCES)

libresources_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -fdiagnostics-show-option -Wno-long-long
