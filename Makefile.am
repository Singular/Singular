ACLOCAL_AMFLAGS = -I m4

SUBDIRS = resources omalloc factory \
libpolys gfanlib IntegerProgramming \
kernel Singular \
dox emacs redhat desktop

DIST_SUBDIRS = $(SUBDIRS) doc
if ENABLE_DOC_BUILD
SUBDIRS += doc
endif

EXTRA_DIST = README README.md README.pkg autogen.sh doxy redhat/singular.spec.in GPL2 GPL3

bin_SCRIPTS = libsingular-config

install-data-local:
	if test -e $(srcdir)/doc/doc.tbz2; then\
	   test -e $(DESTDIR)$(datadir) || mkdir -p $(DESTDIR)$(datadir);\
	   test -e $(DESTDIR)$(datadir)/info || mkdir $(DESTDIR)$(datadir)/info;\
	   test -e $(DESTDIR)$(datadir)/singular || mkdir $(DESTDIR)$(datadir)/singular;\
	   test -e $(DESTDIR)$(htmldir) || mkdir -p $(DESTDIR)$(htmldir);\
	   (cat ${top_srcdir}/doc/doc.tbz2| (cd $(DESTDIR)$(htmldir);tar -jx --no-same-owner -f -));\
	   mv  $(DESTDIR)$(htmldir)/singular.info  $(DESTDIR)$(datadir)/info/.;\
	   mv  $(DESTDIR)$(htmldir)/singular.idx  $(DESTDIR)$(datadir)/singular/.;\
	fi

uninstall-local:
	-(cd $(prefix); rm -rf singular info/singular.info)

configheaderdir = ${includedir}/singular
nodist_configheader_HEADERS = singularconfig.h
DISTCLEANFILES = $(nodist_configheader_HEADERS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Singular.pc
