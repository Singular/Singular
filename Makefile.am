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
	   test -e $(prefix)$(datadir) || mkdir -p $(prefix)$(datadir);\
	   test -e $(prefix)$(datadir)/info || mkdir $(prefix)$(datadir)/info;\
	   test -e $(prefix)$(datadir)/singular || mkdir $(prefix)$(datadir)/singular;\
	   test -e $(prefix)$(htmldir) || mkdir -p $(prefix)$(htmldir);\
	   (cat ${top_srcdir}/doc/doc.tbz2| (cd $(prefix)$(htmldir);tar -jx --no-same-owner -f -));\
	   mv  $(prefix)$(htmldir)/singular.info  $(prefix)$(datadir)/info/.;\
	   mv  $(prefix)$(htmldir)/singular.idx  $(prefix)$(datadir)/singular/.;\
	fi

uninstall-local:
	-(cd $(prefix)$(datadir); rm -rf singular info/singular.info)

configheaderdir = ${includedir}/singular
nodist_configheader_HEADERS = singularconfig.h
DISTCLEANFILES = $(nodist_configheader_HEADERS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Singular.pc
