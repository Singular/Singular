AC_INIT([singular], [3.1.2.sw])
AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([reporter/reporter.h])
AC_CONFIG_HEADER([config.h misc/auxiliary.h])
AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE

dnl Check if build env is sane
AM_SANITY_CHECK

# Turn off shared libraries during beta-testing, since they
# make the build process take too long.
LT_INIT([shared])
# LT_INIT(dlopen disable-static) # doesn't work on PowerPc!

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_INSTALL
AM_PROG_CC_C_O

# Checks for libraries.
LB_CHECK_GMP(3.1.1,,AC_MSG_ERROR([Unable to find GMP on your machine: please use --with-gmp=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))
LB_CHECK_NTL(5.0,,AC_MSG_WARN([Unable to find NTL (which is strongly recommended) on your machine: please use --with-ntl=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([memmove memset pow sqrt strchr])
AC_FUNC_VPRINTF
AC_FUNC_MALLOC
AC_CHECK_FUNC([vsnprintf], [AC_DEFINE([HAVE_VSNPRINTF], [1], [Define if vsnprintf exists.])])

# Checks for header files.
AC_HEADER_STDC
AC_STDC_HEADERS
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

dnl INCLUDES=""
dnl dnl OS specific flags and options
dnl case "$host" in
dnl     *-*-freebsd*)
dnl         LIBS="$LIBS -L/usr/local/lib -lc"
dnl 	CFLAGS="$CFLAGS -I/usr/local/include"
dnl 	INCLUDES="$INCLUDES -I/usr/local/include"
dnl 	;;
dnl esac	
dnl AC_SUBST(INCLUDES)

SING_CHECK_P_PROCS
SING_CHECK_ARITH_RINGS
SING_CHECK_FACTORY
SING_CHECK_PLURAL

if test "x$prefix" != xNONE; then
  PREFIX="$prefix"
else
  PREFIX="$ac_default_prefix"
fi

# AC_SUBST(PREFIX)
AC_DEFINE_UNQUOTED(INSTALL_PREFIX,"$PREFIX",Prefix)

AC_DEFINE([SINGULAR],[1],[SINGULAR])
AC_DEFINE([DISABLE_GMP_CPP],[1],[DISABLE_GMP_CPP])
AC_DEFINE([NOSTREAMIO],[1],[DISABLE_GMP_CPP])

## AC_DEFINE([HAVE_POLYEXTENSIONS], [1], [Enable the algebraic & transcendental extensions])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([misc/Makefile])
AC_CONFIG_FILES([reporter/Makefile])
AC_CONFIG_FILES([resources/Makefile])
AC_CONFIG_FILES([coeffs/Makefile])
AC_CONFIG_FILES([polys/Makefile])
AC_CONFIG_FILES([tests/Makefile])
AC_CONFIG_FILES([libpolys-config])
AC_OUTPUT
