#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([coeffs], [3.1.2.sw])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([coeffs.h])
AC_CONFIG_HEADERS([config.h])

#check for host:
AC_CANONICAL_HOST
case $host_os in
  *cygwin* ) LIBPREFIX="-Xlinker -Bdynamic";;
  * ) LIBPREFIX="";;
esac

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_INSTALL
AC_PROG_RANLIB
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([float.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([memmove memset pow sqrt strchr])

AC_SUBST(LIBPREFIX)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
