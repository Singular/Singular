#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([feFopen.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([sys/param.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

AC_MSG_RESULT($ac_cv_singuname)
if test "$ac_cv_singuname" = unknown; then
  AC_MSG_WARN(Unknown architecture: Check singuname.sh)
  ac_cv_singuname="unknown"
fi
AC_DEFINE_UNQUOTED(S_UNAME, "$ac_cv_singuname", Singular's own uname\, believe it or not)
SINGUNAME=$ac_cv_singuname
AC_SUBST(SINGUNAME)


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
