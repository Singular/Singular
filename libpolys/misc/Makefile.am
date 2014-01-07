ACLOCAL_AMFLAGS = -I ../../m4

# CXXTEMPLFLAGS      =-ftrapv -fno-implicit-templates

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..  -DHAVE_CONFIG_H

# if WANT_DEBUG
#   LIB_G=libmisc_g.la
# else
#   LIB_G=
# endif

libmisc_LTLIBRARIES = libmisc.la 
# ${LIB_G}
libmiscdir = $(libdir)/singular

# libmisc_la_CFLAGS   = -O3 -fomit-frame-pointer ${PIPE}
# libmisc_g_la_CFLAGS = ${PIPE}
# 
# libmisc_la_CXXFLAGS   = -O3 -fomit-frame-pointer ${PIPE} ${CXXTEMPLFLAGS}
# libmisc_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} ${CXXTEMPLFLAGS}
# # -fdiagnostics-show-option 
 
# libmisc_la_CPPFLAGS   = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DSING_NDEBUG -DOM_NDEBUG
# libmisc_g_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H
# libmisc_la_CPPFLAGS = ${AM_CPPFLAGS}


noinst_LTLIBRARIES = libintvec.la
# libintvec_g.la

libmisc_la_LIBADD = libintvec.la
# libmisc_g_la_LIBADD = libintvec_g.la

# libmisc_la_LDFLAGS    = -release ${PACKAGE_VERSION}

SOURCES  = int64vec.cc options.c
libmisc_la_SOURCES   = $(SOURCES)
# libmisc_g_la_SOURCES = $(SOURCES)


libmisc_includedir = $(includedir)/singular/misc
libmisc_include_HEADERS = mylimits.h options.h intvec.h int64vec.h
nodist_libmisc_include_HEADERS = auxiliary.h 

nodist_libmisc_la_SOURCES   = auxiliary.h 
# nodist_libmisc_g_la_SOURCES = auxiliary.h 


libintvec_la_SOURCES = intvec.cc 
# libintvec_g_la_SOURCES = ${libintvec_la_SOURCES}


# libintvec_la_CXXFLAGS = -O3 -fomit-frame-pointer ${PIPE} -fwrapv -fno-implicit-templates
# libintvec_g_la_CXXFLAGS = -g -Wextra -Wall -pedantic -Wno-long-long ${PIPE} -fwrapv -fno-implicit-templates

# libintvec_la_CPPFLAGS   = ${AM_CPPFLAGS} -DHAVE_CONFIG_H -DSING_NDEBUG -DOM_NDEBUG
# libintvec_g_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H
# libintvec_la_CPPFLAGS = ${AM_CPPFLAGS} -DHAVE_CONFIG_H

