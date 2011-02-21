lib_LIBRARIES=libmisc.a libmisc_g.a

AM_CFLAGS	= -O3 -w -fomit-frame-pointer ${PIPE}
CXXTEMPLFLAGS	= -fno-implicit-templates --no-exceptions
AM_CXXFLAGS	= -O3 -w -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
INCLUDES	= -I${top_srcdir}/.. -I${top_srcdir}/../.. -I${top_builddir}/..
AM_CPPFLAGS	= -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H

SOURCES = intvec.cc

libmisc_a_SOURCES = $(SOURCES)

libmisc_a_includedir=$(includedir)/libpolys/misc

libmisc_a_include_HEADERS = auxiliary.h mylimits.h options.h intvec.h

libmisc_g_a_SOURCES = $(SOURCES)

libmisc_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long
# -fdiagnostics-show-option 
