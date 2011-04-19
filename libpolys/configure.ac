AC_INIT([singular], [3.1.2.sw])
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([../m4],[m4])
AC_CONFIG_SRCDIR([reporter/reporter.h])
AC_CONFIG_HEADER([config.h])

# misc/auxiliary.h

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_INSTALL
AM_PROG_CC_C_O
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

# Checks for libraries.
LB_CHECK_GMP(,,AC_MSG_ERROR([Unable to find GMP on your machine: please use --with-gmp=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([memmove memset pow sqrt strchr])
AC_FUNC_VPRINTF
AC_FUNC_MALLOC
AC_CHECK_FUNC([vsnprintf], [AC_DEFINE([HAVE_VSNPRINTF], [1],
                               [Define if vsnprintf exists.])])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h pwd.h sys/param.h limits.h float.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

SING_CHECK_PIPE

# check for cpu properties
AC_CHECK_SIZEOF(long,4)
SING_CHECK_CPU

#check for host:
AC_CANONICAL_HOST
case $host_os in
  *cygwin* ) LIBPREFIX="-Xlinker -Bdynamic";;
  * ) LIBPREFIX="";;
esac

SING_CHECK_P_PROCS

AC_SUBST(LIBPREFIX)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([misc/Makefile])
AC_CONFIG_FILES([reporter/Makefile])
AC_CONFIG_FILES([resources/Makefile])
AC_CONFIG_FILES([coeffs/Makefile])
AC_CONFIG_FILES([polys/Makefile])
AC_CONFIG_FILES([tests/Makefile])
AC_OUTPUT
