AC_INIT([singular], [3.1.2.sw])
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([../m4])

AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.
LB_CHECK_GMP(,,AC_MSG_ERROR([Unable to find GMP on your machine: please use --with-gmp=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([tests/Makefile])
AC_CONFIG_SUBDIRS([misc reporter resources coeffs polys])
AC_OUTPUT
