ACLOCAL_AMFLAGS = -I ../m4

emacsdir=$(datadir)/singular/emacs

EMACS = \
    ChangeLog \
    COPYING \
    NEWS \
    singular.el \
    singular.xpm  .emacs-general  .emacs-singular

#++ hlp-cmpl.el lib-cmpl.el ex-cmpl.el cmd-cmpl.el ??
#-- BUGS     cmpl.pl     ESingular.emacs21     lib.pl     Makefile.in     singular.el.decl

dist_emacs_DATA = $(EMACS)

# test.sing
# TODO: remove this!
#all-local:
#	[ -d ${builddir} ] || ln -s ${srcdir} ${builddir}

EXTRA_DIST = $(EMACS)
