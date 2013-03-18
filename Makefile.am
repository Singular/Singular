ACLOCAL_AMFLAGS = -I m4

USE_FACTORY =
if ENABLE_FACTORY
  USE_FACTORY += factory
endif

PACKAGES=findexec omalloc $(USE_FACTORY) libpolys kernel numeric Singular IntegerProgramming gfanlib dyn_modules 

SUBDIRS =$(PACKAGES)
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

