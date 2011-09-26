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
