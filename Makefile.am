ACLOCAL_AMFLAGS = -I m4

USE_FACTORY =
if ENABLE_FACTORY
  USE_FACTORY += factory
endif

PACKAGES=resources omalloc xalloc $(USE_FACTORY) \
libpolys kernel numeric gfanlib dyn_modules \
IntegerProgramming Singular dox \
emacs debian redhat desktop 

SUBDIRS  = $(PACKAGES)
TESTDIRS = omalloc $(USE_FACTORY) libpolys kernel Singular
#$(PACKAGES)

EXTRA_DIST = README autogen.sh

bin_SCRIPTS = libsingular-config
