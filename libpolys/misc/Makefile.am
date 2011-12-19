noinst_LTLIBRARIES = libmisc.la libmisc_g.la
# libmiscdir = $(libdir)/singular

CXXTEMPLFLAGS      =  ## -fno-implicit-templates

libmisc_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
libmisc_g_la_CFLAGS = ${PIPE}

libmisc_la_CXXFLAGS   = -O3 -fomit-frame-pointer ${PIPE} ${CXXTEMPLFLAGS}
libmisc_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# -fdiagnostics-show-option 

libmisc_la_CPPFLAGS   = -DHAVE_CONFIG_H -DNDEBUG -DOM_NDEBUG
libmisc_g_la_CPPFLAGS = -DHAVE_CONFIG_H

INCLUDES = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..

SOURCES  = intvec.cc int64vec.cc options.c
libmisc_la_SOURCES   = $(SOURCES)
libmisc_g_la_SOURCES = $(SOURCES)


libmisc_includedir = $(includedir)/singular/misc
libmisc_include_HEADERS = mylimits.h options.h intvec.h int64vec.h

nodist_libmisc_la_SOURCES   = auxiliary.h 
nodist_libmisc_g_la_SOURCES = auxiliary.h 




