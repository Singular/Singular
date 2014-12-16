ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..

noinst_LTLIBRARIES = libmisc.la  libintvec.la
##### libmiscdir = $(libdir)/singular

libmisc_la_LIBADD = libintvec.la

## libmisc_la_LDFLAGS    = -release ${PACKAGE_VERSION}

libmisc_la_SOURCES   = int64vec.cc options.c sirandom.c

libmisc_la_includedir = $(includedir)/singular/misc
libmisc_la_include_HEADERS = mylimits.h options.h intvec.h int64vec.h sirandom.h

nodist_libmisc_la_include_HEADERS = auxiliary.h
nodist_libmisc_la_SOURCES   = auxiliary.h

libintvec_la_SOURCES = intvec.cc


