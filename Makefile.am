ACLOCAL_AMFLAGS = -I m4

if ENABLE_FACTORY
  USE_FACTORY = factory
endif

SUBDIRS = resources omalloc xalloc $(USE_FACTORY) \
libpolys gfanlib IntegerProgramming \
kernel Singular \
dox emacs desktop

EXTRA_DIST = README README.md README.pkg autogen.sh git-version-gen doxy singular.spec.in

bin_SCRIPTS = libsingular-config

man_MANS = libsingular-config.1

AM_H2MFLAGS = \
	--manual="CAS Singular" \
	--source="Singular" \
	--libtool \
	--no-info

## short descriptions for manpages
libsingular_config_DESCRIPTION = "get information about installed Singular library"

dist-hook:
	$(srcdir)/git-version-gen $(srcdir)/.tarball-git-version > $(distdir)/.tarball-git-version


configheaderdir = ${includedir}/singular
nodist_configheader_HEADERS = singularconfig.h
DISTCLEANFILES = $(nodist_configheader_HEADERS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Singular.pc

%.1: %
	$(HELP2MAN) \
			-s 1 \
			$(AM_H2MFLAGS) \
			-I $(top_srcdir)/man/$*.h2m \
			-n $(if $($(subst -,_,$*)_DESCRIPTION), $($(subst -,_,$*)_DESCRIPTION), "manual page for $*") \
			-o $@ \
		$(abs_top_builddir)/$<
