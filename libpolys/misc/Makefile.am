ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/.. \
$(FACTORY_INCLUDES)

noinst_LTLIBRARIES = libmisc.la
##### libmiscdir = $(libdir)/singular

## libmisc_la_LDFLAGS    = -release ${PACKAGE_VERSION}

libmisc_la_SOURCES   = int64vec.cc options.c sirandom.c intvec.cc prime.cc
libmisc_la_LDFLAGS = $(SINGULAR_LDFLAGS)

libmisc_la_includedir = $(includedir)/singular/misc
libmisc_la_include_HEADERS = mylimits.h options.h intvec.h int64vec.h \
                             sirandom.h prime.h auxiliary.h distrib.h

