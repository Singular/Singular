ALLOC=omalloc


if ENABLE_FACTORY
  USE_FACTORY = factory 
else
  USE_FACTORY =
endif
 
PACKAGES=$(ALLOC) $(USE_FACTORY) libpolys numeric kernel Singular

SUBDIRS=$(PACKAGES)

TESTDIRS=$(ALLOC) libpolys kernel Singular
