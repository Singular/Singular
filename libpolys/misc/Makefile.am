ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..  

libmisc_LTLIBRARIES = libmisc.la 
libmiscdir = $(libdir)/singular

noinst_LTLIBRARIES = libintvec.la

libmisc_la_LIBADD = libintvec.la \
${top_builddir}/../omalloc/libomalloc.la ${top_builddir}/../resources/libresources.la

# libmisc_la_LDFLAGS    = -release ${PACKAGE_VERSION}

libmisc_la_SOURCES   = int64vec.cc options.c

libmisc_includedir = $(includedir)/singular/misc
libmisc_include_HEADERS = mylimits.h options.h intvec.h int64vec.h
nodist_libmisc_include_HEADERS = auxiliary.h 

nodist_libmisc_la_SOURCES   = auxiliary.h 

libintvec_la_SOURCES = intvec.cc 


