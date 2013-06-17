ACLOCAL_AMFLAGS = -I m4

USE_FACTORY =
if ENABLE_FACTORY
  USE_FACTORY += factory
endif

PACKAGES=resources omalloc xalloc $(USE_FACTORY) libpolys kernel numeric gfanlib dyn_modules IntegerProgramming Singular dox

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

DEBIAN = README \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/rules \
	debian/singular-bin.install \
	debian/singular-common.dirs \
	debian/singular-common.install \
	debian/singular-dev.install \
	debian/source/format \
	debian/source/include-binaries

REDHAT = README \
	redhat/singular.spec

iconsdir = $(datadir)/icons
icons_DATA = desktop/Singular.png

desktopdir = $(datadir)/applications
desktop_DATA = desktop/Singular.desktop desktop/Singular-manual.desktop

substvars = sed \
	-e 's|@docdir[@]|$(docdir)|g' \
	-e 's|@datadir[@]|$(datadir)|g' \
	-e 's|@PACKAGE_VERSION[@]|$(PACKAGE_VERSION)|g'
desktop/Singular.desktop: desktop/Singular.desktop.in
	$(AM_V_GEN) $(substvars) $< > $@
desktop/Singular-manual.desktop: desktop/Singular-manual.desktop.in
	$(AM_V_GEN) $(substvars) $< > $@
desktop_in = desktop/Singular.desktop.in desktop/Singular-manual.desktop.in

EXTRA_DIST = autogen.sh $(EMACS) \
	$(DEBIAN) $(REDHAT) \
	$(icons_DATA) $(desktop_in) 

bin_SCRIPTS = libsingular-config

