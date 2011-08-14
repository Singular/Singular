ALLOC=omalloc


if ENABLE_FACTORY
  USE_FACTORY = factory 
else
  USE_FACTORY =
endif
 
PACKAGES=$(ALLOC) $(USE_FACTORY) libpolys kernel Singular

SUBDIRS=$(PACKAGES)

TESTDIRS=libpolys kernel Singular
