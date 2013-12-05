ACLOCAL_AMFLAGS = -I ../m4

iconsdir = $(datadir)/icons
icons_DATA = Singular.png

desktopdir = $(datadir)/applications
desktop_DATA = Singular.desktop Singular-manual.desktop
desktop_in = Singular.desktop.in Singular-manual.desktop.in

substvars = sed \
	-e 's![@]docdir[@]!$(docdir)!g' \
	-e 's![@]datadir[@]!$(datadir)!g' \
	-e 's![@]PACKAGE_VERSION[@]!$(PACKAGE_VERSION)!g'

Singular.desktop: $(abs_srcdir)/Singular.desktop.in
	$(AM_V_GEN) $(substvars) $(abs_srcdir)/Singular.desktop.in > $@

Singular-manual.desktop: $(abs_srcdir)/Singular-manual.desktop.in
	$(AM_V_GEN) $(substvars) $(abs_srcdir)/Singular-manual.desktop.in > $@

EXTRA_DIST = $(icons_DATA) $(desktop_in)

CLEANFILES = $(desktop_DATA)
