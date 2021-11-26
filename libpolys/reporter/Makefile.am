ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = -I${top_srcdir} -I${top_srcdir}/.. -I${top_builddir} -I${top_builddir}/..  $(FACTORY_INCLUDES)  ${FLINT_CFLAGS}  ${NTL_CPPFLAGS} ${GMP_CPPFLAGS}

noinst_LTLIBRARIES = libreporter.la
#### libreporterdir = $(libdir)/singular

libreporter_la_SOURCES  = dError.cc reporter.cc s_buff.cc
libreporter_la_LDFLAGS = $(SINGULAR_LDFLAGS)
## libreporter_la_LDFLAGS  = -release ${PACKAGE_VERSION}
## libreporter_la_LIBADD   = ${top_builddir}/misc/libmisc.la

libreporter_la_includedir  =$(includedir)/singular/reporter
libreporter_la_include_HEADERS   = reporter.h s_buff.h si_signals.h


