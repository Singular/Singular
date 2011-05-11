lib_LIBRARIES=libmisc.a libmisc_g.a

CXXTEMPLFLAGS      = -fno-implicit-templates --no-exceptions

libmisc_a_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libmisc_g_a_CFLAGS = ${PIPE}

libmisc_a_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
libmisc_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

libmisc_a_CPPFLAGS   = -DNDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H
libmisc_g_a_CPPFLAGS = -DHAVE_CONFIG_H

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..

SOURCES  = intvec.cc options.c
libmisc_a_SOURCES   = $(SOURCES)
libmisc_g_a_SOURCES = $(SOURCES)

libmisc_a_includedir  =$(includedir)/libpolys/misc
libmisc_g_a_includedir=$(includedir)/libpolys/misc

MISCHEADERS  = auxiliary.h mylimits.h options.h intvec.h
libmisc_a_include_HEADERS   = $(MISCHEADERS)
libmisc_g_a_include_HEADERS = $(MISCHEADERS)



