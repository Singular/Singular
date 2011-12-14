ACLOCAL_AMFLAGS = -I m4

ALLOC=omalloc

if ENABLE_FACTORY
  USE_FACTORY = factory 
else
  USE_FACTORY =
endif
 
PACKAGES=findexec $(ALLOC) $(USE_FACTORY) libpolys kernel numeric Singular

SUBDIRS=$(PACKAGES)

TESTDIRS=$(ALLOC) $(USE_FACTORY) libpolys kernel Singular

EXTRA_DIST = autogen.sh

bin_SCRIPTS = libsingular-config
