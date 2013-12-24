ACLOCAL_AMFLAGS = -I m4

if ENABLE_FACTORY
  USE_FACTORY = factory
endif

SUBDIRS = resources omalloc xalloc $(USE_FACTORY) \
libpolys kernel numeric gfanlib dyn_modules \
IntegerProgramming Singular dox \
emacs debian redhat desktop

# PACKAGES = $(SUBDIRS)
# TESTDIRS = omalloc $(USE_FACTORY) libpolys kernel Singular dyn_modules

EXTRA_DIST = README autogen.sh git-version-gen

bin_SCRIPTS = libsingular-config

dist-hook:
	$(srcdir)/git-version-gen $(srcdir)/.tarball-git-version > $(distdir)/.tarball-git-version


configheaderdir = ${includedir}/singular
nodist_configheader_HEADERS = singularconfig.h

dist_man1_MANS = doc/singular.man

