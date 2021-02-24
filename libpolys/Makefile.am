ACLOCAL_AMFLAGS = -I ../m4

bin_SCRIPTS = libpolys-config

SUBDIRS=misc reporter coeffs polys tests

configheaderdir = ${includedir}/singular
nodist_configheader_HEADERS = libpolysconfig.h
DISTCLEANFILES = $(nodist_configheader_HEADERS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libpolys.pc
