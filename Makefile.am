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

EXTRA_DIST = README autogen.sh

bin_SCRIPTS = libsingular-config
