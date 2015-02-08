ACLOCAL_AMFLAGS = -I ../m4

bin_SCRIPTS = libpolys-config

man_MANS = libpolys-config.1

AM_H2MFLAGS = \
	--manual="CAS Singular" \
	--source="Singular" \
	--libtool \
	--no-info

## short descriptions for manpages
libpolys_config_DESCRIPTION = "get information about installed Singular polynomial library"

SUBDIRS=misc reporter coeffs polys tests

configheaderdir = ${includedir}/singular
nodist_configheader_HEADERS = libpolysconfig.h
DISTCLEANFILES = $(nodist_configheader_HEADERS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libpolys.pc

%.1: %
	$(HELP2MAN) \
			-s 1 \
			$(AM_H2MFLAGS) \
			-I $(top_srcdir)/man/$*.h2m \
			-n $(if $($(subst -,_,$*)_DESCRIPTION), $($(subst -,_,$*)_DESCRIPTION), "manual page for $*") \
			-o $@ \
		$(abs_top_builddir)/$<
