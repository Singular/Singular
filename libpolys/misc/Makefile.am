libmisc_LIBRARIES = libmisc.a libmisc_g.a
libmiscdir = $(libdir)/singular

CXXTEMPLFLAGS      =  --no-exceptions ## -fno-implicit-templates

libmisc_a_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libmisc_g_a_CFLAGS = ${PIPE}

libmisc_a_CXXFLAGS   = -O3 -fomit-frame-pointer --no-rtti ${PIPE} ${CXXTEMPLFLAGS}
libmisc_g_a_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

libmisc_a_CPPFLAGS   = -DNDEBUG -DOM_NDEBUG
#libmisc_g_a_CPPFLAGS =

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..

SOURCES  = intvec.cc int64vec.cc options.c
libmisc_a_SOURCES   = $(SOURCES)
libmisc_g_a_SOURCES = $(SOURCES)

libmisc_includedir = $(includedir)/singular/misc
libmisc_include_HEADERS = auxiliary.h mylimits.h options.h intvec.h int64vec.h




