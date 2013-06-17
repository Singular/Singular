ACLOCAL_AMFLAGS = -I ../m4

iconsdir = $(datadir)/icons
icons_DATA = Singular.png

desktopdir = $(datadir)/applications
desktop_DATA = $(builddir)/Singular.desktop $(builddir)/Singular-manual.desktop

substvars = sed \
	-e 's![@]docdir[@]!$(docdir)!g' \
	-e 's![@]datadir[@]!$(datadir)!g' \
	-e 's![@]PACKAGE_VERSION[@]!$(PACKAGE_VERSION)!g'

Singular.desktop: $(abs_srcdir)/Singular.desktop.in Makefile
	$(AM_V_GEN) $(substvars) $< > $@

#	$(do_subst) < $(srcdir)/automake.in > automake

Singular-manual.desktop: $(abs_srcdir)/Singular-manual.desktop.in Makefile
	$(AM_V_GEN) $(substvars) $< > $@

desktop_in = Singular.desktop.in Singular-manual.desktop.in

EXTRA_DIST = $(icons_DATA) $(desktop_in)

CLEANFILES = Singular.desktop Singular-manual.desktop
