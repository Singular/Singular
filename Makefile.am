ACLOCAL_AMFLAGS = -I m4

if ENABLE_FACTORY
  USE_FACTORY = factory
endif

if HAVE_GFANLIB 
  USE_GFANLIB = gfanlib
endif




SUBDIRS = resources omalloc xalloc $(USE_FACTORY) \
libpolys kernel numeric $(USE_GFANLIB) dyn_modules \
IntegerProgramming Singular dox \
emacs debian redhat desktop

# PACKAGES = $(SUBDIRS)
# TESTDIRS = omalloc $(USE_FACTORY) libpolys kernel Singular dyn_modules

EXTRA_DIST = README autogen.sh git-version-gen

bin_SCRIPTS = libsingular-config

dist-hook:
	$(srcdir)/git-version-gen $(srcdir)/.tarball-git-version > $(distdir)/.tarball-git-version


configheaderdir = ${includedir}/singular/
nodist_configheader_HEADERS = singularconfig.h

