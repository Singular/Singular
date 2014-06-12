ACLOCAL_AMFLAGS = -I m4

if ENABLE_FACTORY
  USE_FACTORY = factory
endif

SUBDIRS = resources omalloc xalloc $(USE_FACTORY) \
libpolys gfanlib IntegerProgramming \
kernel Singular \
dox emacs debian redhat desktop

EXTRA_DIST = README autogen.sh git-version-gen

bin_SCRIPTS = libsingular-config

dist-hook:
	$(srcdir)/git-version-gen $(srcdir)/.tarball-git-version > $(distdir)/.tarball-git-version


configheaderdir = ${includedir}/singular
nodist_configheader_HEADERS = singularconfig.h
DISTCLEANFILES = $(nodist_configheader_HEADERS)

dist_man1_MANS = doc/Singular.man doc/ESingular.man doc/TSingular.man

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Singular.pc
