ACLOCAL_AMFLAGS = -I m4

ALLOC=omalloc

if ENABLE_FACTORY
  USE_FACTORY = factory 
else
  USE_FACTORY =
endif
 
PACKAGES=$(ALLOC) $(USE_FACTORY) libpolys kernel numeric Singular

SUBDIRS=$(PACKAGES)

TESTDIRS=$(ALLOC) $(USE_FACTORY) libpolys kernel Singular

# libfac doesn't use automake, just include it all in tarball
EXTRA_DIST = libfac autogen.sh
