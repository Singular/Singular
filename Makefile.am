ACLOCAL_AMFLAGS = -I m4

ALLOC=omalloc

if ENABLE_FACTORY
  USE_FACTORY = factory 
else
  USE_FACTORY =
endif
 
PACKAGES=findexec $(ALLOC) $(USE_FACTORY) libpolys kernel numeric Singular IntegerProgramming

SUBDIRS=$(PACKAGES)

TESTDIRS=$(ALLOC) $(USE_FACTORY) libpolys kernel Singular


emacsdir=$(datadir)/emacs
dist_emacs_DATA =  emacs/BUGS \
    emacs/ChangeLog \
    emacs/cmpl.pl \
    emacs/COPYING \
    emacs/ESingular.emacs21 \
    emacs/lib.pl \
    emacs/Makefile.in \
    emacs/NEWS \
    emacs/singular.el \
    emacs/singular.el.decl \
    emacs/singular.xpm

# emacs/test.sing
# TODO: remove this!
#all-local:
#	[ -d ${builddir}/emacs ] || ln -s ${srcdir}/emacs ${builddir}


EXTRA_DIST = autogen.sh emacs


bin_SCRIPTS = libsingular-config
