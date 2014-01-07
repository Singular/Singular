ACLOCAL_AMFLAGS = -I ../../m4

# CXXTEMPLFLAGS =-ftrapv  
## -fno-implicit-templates

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..  -DHAVE_CONFIG_H

# if WANT_DEBUG
#   LIB_G=libreporter_g.la
# else
#   LIB_G=
# endif

libreporter_LTLIBRARIES = libreporter.la 
# ${LIB_G}
libreporterdir = $(libdir)/singular

# libreporter_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
# libreporter_g_la_CFLAGS = ${PIPE}
# 
# libreporter_la_CXXFLAGS   = -O3 -fomit-frame-pointer ${PIPE} ${CXXTEMPLFLAGS}
# libreporter_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# # -fdiagnostics-show-option 

# libreporter_la_CPPFLAGS   = ${AM_CPPFLAGS} -DSING_NDEBUG -DOM_NDEBUG -DHAVE_CONFIG_H
# libreporter_g_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H
# libreporter_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H


SOURCES  = dError.cc reporter.cc
libreporter_la_SOURCES   = $(SOURCES)
# libreporter_g_la_SOURCES = $(SOURCES)

## libreporter_la_LDFLAGS    = -release ${PACKAGE_VERSION}

libreporter_includedir  =$(includedir)/singular/reporter

libreporter_include_HEADERS   = reporter.h 



