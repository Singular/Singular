ACLOCAL_AMFLAGS = -I m4

if ENABLE_FACTORY
  USE_FACTORY = factory 
else
  USE_FACTORY =
endif
 
PACKAGES=findexec omalloc $(USE_FACTORY) libpolys kernel numeric Singular IntegerProgramming dyn_modules

SUBDIRS=$(PACKAGES)

TESTDIRS=$(PACKAGES)


emacsdir=$(datadir)/singular/emacs

EMACS = \
    emacs/ChangeLog \
    emacs/COPYING \
    emacs/NEWS \
    emacs/singular.el \
    emacs/singular.xpm  emacs/.emacs-general  emacs/.emacs-singular

#++ hlp-cmpl.el lib-cmpl.el ex-cmpl.el cmd-cmpl.el ??
#-- emacs/BUGS     emacs/cmpl.pl     emacs/ESingular.emacs21     emacs/lib.pl     emacs/Makefile.in     emacs/singular.el.decl 

dist_emacs_DATA = $(EMACS)

# emacs/test.sing
# TODO: remove this!
#all-local:
#	[ -d ${builddir}/emacs ] || ln -s ${srcdir}/emacs ${builddir}


EXTRA_DIST = autogen.sh $(EMACS)


bin_SCRIPTS = libsingular-config

