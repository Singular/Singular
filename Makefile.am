ACLOCAL_AMFLAGS = -I m4

if ENABLE_OMALLOC
  USE_OMALLOC = omalloc
else
  USE_OMALLOC = xalloc
endif

SUBDIRS = resources $(USE_OMALLOC) factory \
libpolys gfanlib IntegerProgramming \
kernel Singular \
dox emacs redhat desktop

EXTRA_DIST = README README.md README.pkg autogen.sh git-version-gen doxy redhat/singular.spec.in GPL2 GPL3 doc/doc.tbz2

bin_SCRIPTS = libsingular-config

dist-hook:
	$(srcdir)/git-version-gen $(srcdir)/.tarball-git-version > $(distdir)/.tarball-git-version

install-data-local:
	if test -e $(srcdir)/doc/doc.tbz2; then\
	  (cat ${top_srcdir}/doc/doc.tbz2| (cd $(DESTDIR)$(datadir)/singular; tar jxf -));\
	   mkdir $(DESTDIR)$(datadir)/info;\
	   mkdir $(DESTDIR)$(datadir)/doc;\
	   mv  $(DESTDIR)$(datadir)/singular/singular.hlp  $(DESTDIR)$(datadir)/info/.;\
	   mv  $(DESTDIR)$(datadir)/singular/singular.idx  $(DESTDIR)$(datadir)/doc/.;\
	fi

uninstall-local:
	-(cd $(DESTDIR)$(datadir); rm -rf singular info/singular.hlp)

configheaderdir = ${includedir}/singular
nodist_configheader_HEADERS = singularconfig.h
DISTCLEANFILES = $(nodist_configheader_HEADERS)

dist_man1_MANS = doc/Singular.man doc/ESingular.man doc/TSingular.man

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Singular.pc
