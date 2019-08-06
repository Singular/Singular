ACLOCAL_AMFLAGS = -I ../m4

emacsdir=$(datadir)/singular/emacs

EMACS = \
    COPYING \
    singular.el \
    singular.xpm  .emacs-general  .emacs-singular \
    cmd-cmpl.el ex-cmpl.el hlp-cmpl.el lib-cmpl.el
#++ hlp-cmpl.el lib-cmpl.el ex-cmpl.el cmd-cmpl.el: for now from Master Singular


#-- BUGS     cmpl.pl     ESingular.emacs21     lib.pl     Makefile.in     singular.el.decl

dist_emacs_DATA = $(EMACS)

# TODO: remove this!
#all-local:
#	[ -d ${builddir} ] || ln -s ${srcdir} ${builddir}

EXTRA_DIST = $(EMACS)
