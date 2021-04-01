AC_INIT([libpolys], [4.2.0])

AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_SRCDIR([reporter/reporter.h])
AC_CONFIG_HEADER([_config.h])

AM_MAINTAINER_MODE([enable])

AM_INIT_AUTOMAKE([-Wall foreign subdir-objects]) # -Wno-extra-portability -Werror silent-rules
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

dnl Check if build env is sane
AM_SANITY_CHECK

AX_PROG_CXX_FOR_BUILD

# Add pre'prefixed config
AX_PREFIX_CONFIG_H([libpolysconfig.h],[],[_config.h])

# - Check for CC and CXX but be careful about CFLAGS.
SING_RESET_FLAGS()
SING_CHECK_SET_ARGS()

AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LN_S
AC_PROG_INSTALL

# Turn off shared libraries during beta-testing, since they
# make the build process take too long.
LT_INIT([win32-dll])
# ([shared])
# LT_INIT([win32-dll])(dlopen disable-static) # doesn't work on PowerPC!

SING_WIN_FLAGS

# Checks for libraries.
# This test for -lpthread etc has to come before AX_PTHREAD,
# because libtool tends to ignore -pthread in linking shared C++-libs
# see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=25460
# (happens with ubuntu 14.04)
AC_SEARCH_LIBS(sem_wait,[rt pthreads pthread],[],[
  AC_MSG_ERROR([sem_wait not found in rt,pthreads,pthread])
])

# Check whether --with-gmp was given.
AC_ARG_WITH([gmp],[AS_HELP_STRING([--with-gmp=path],
                    [provide a non-standard location of gmp])])
if test "${with_gmp+set}" = set; then :
  if test "$with_gmp" = yes ; then
	GMP_HOME_PATH="${DEFAULT_CHECKING_PATH}"
   elif test "$with_gmp" != no ; then
	GMP_HOME_PATH="$with_gmp"
    fi
else
  GMP_HOME_PATH="${DEFAULT_CHECKING_PATH}"
fi

BACKUP_CFLAGS=${CFLAGS}
BACKUP_LIBS=${LIBS}

for GMP_HOME in ${GMP_HOME_PATH}
do
  if test "x$GMP_HOME" != "x/usr"; then
    if test -e ${GMP_HOME}/include/gmp.h; then
      GMP_CPPFLAGS="-I${GMP_HOME}/include"
      GMP_LIBS="-L${GMP_HOME}/lib -Wl,-rpath -Wl,${GMP_HOME}/lib -lgmp"
      break
    fi
  fi
done
if test -z "${GMP_LIBS}"
then
  GMP_LIBS="-lgmp"
fi

CFLAGS="${BACKUP_CFLAGS} ${GMP_CPPFLAGS}"
LIBS="${BACKUP_LIBS} ${GMP_LIBS}"
AC_SUBST(GMP_CPPFLAGS)
AC_SUBST(GMP_LIBS)

dnl ntl check in factory
LB_CHECK_FLINT(2.4,,AC_MSG_WARN([Unable to find FLINT (which is strongly recommended) on your machine: please use --with-flint=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

SING_CHECK_JULIA

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([memmove memset pow sqrt strchr setenv putenv])
AC_FUNC_VPRINTF
AC_CHECK_FUNC([vsnprintf], [AC_DEFINE([HAVE_VSNPRINTF], [1], [Define if vsnprintf exists.])])

# Checks for header files.
AC_HEADER_STDC
AC_STDC_HEADERS
AC_CHECK_HEADERS([stdlib.h string.h unistd.h pwd.h sys/param.h limits.h float.h execinfo.h])

# If the compiler supports GCC C++ ABI name demangling (has header
#   cxxabi.h and abi::__cxa_demangle() function), define
#   HAVE_GCC_ABI_DEMANGLE
AX_CXX_GCC_ABI_DEMANGLE

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# SING_CHECK_PIPE

# check for cpu properties
AC_CHECK_SIZEOF(long,4)
SING_CHECK_CPU

#check for host:
AC_CANONICAL_HOST

case $host_os in
  *cygwin* ) AX_APPEND_LINK_FLAGS([-Wl,-Bdynamic]);;
esac

dnl INCLUDES=""
dnl dnl OS specific flags and options (does work without the following:)
dnl case "$host" in
dnl     *-*-freebsd*)
dnl         LIBS="$LIBS -L/usr/local/lib -lc"
dnl 	CFLAGS="$CFLAGS -I/usr/local/include"
dnl 	INCLUDES="$INCLUDES -I/usr/local/include"
dnl 	;;
dnl esac
dnl AC_SUBST(INCLUDES)

SING_CHECK_OMALLOC()

ENABLE_RESOURCES="yes"

AC_ARG_VAR( [RESOURCES_INCLUDES], [INCLUDES for libsingular_resources] )
AC_ARG_VAR( [RESOURCES_LIBS], [LIBS for libsingular_resources] )

AC_MSG_CHECKING(whether libsingular_resources should be used)

if test "x$ENABLE_RESOURCES" = xyes;
then
  AC_MSG_RESULT(yes)

  AC_MSG_CHECKING([  RESOURCES_INCLUDES?..])
  AC_MSG_RESULT(${RESOURCES_INCLUDES:-unset})

  AC_MSG_CHECKING([  RESOURCES_LIBS?..])
  AC_MSG_RESULT(${RESOURCES_LIBS:-unset})

  PKG_REQUIRE="$PKG_REQUIRE singular_resources"
  AC_SUBST(PKG_REQUIRE)
else
  AC_MSG_RESULT(no)
fi

AM_CONDITIONAL([ENABLE_RESOURCES],[test "x$ENABLE_RESOURCES" = xyes])

SING_CHECK_FACTORY()

SING_CHECK_P_PROCS
SING_CHECK_ARITH_RINGS
SING_CHECK_PLURAL

# AC_SUBST(PREFIX)
# AC_DEFINE_UNQUOTED(INSTALL_PREFIX,"$PREFIX",Prefix)

AC_DEFINE([SINGULAR],[1],[SINGULAR])
AC_DEFINE([DISABLE_GMP_CPP],[1],[DISABLE_GMP_CPP])
AC_DEFINE([NOSTREAMIO],[1],[DISABLE_GMP_CPP])

# AX_PYTHON_DEFAULT()
# AX_PYTHON_WITH_VERSION([2.4])

AC_DEFINE_UNQUOTED([NTL_CPPFLAGS],"$NTL_CPPFLAGS",[NTL_CPPFLAGS])
AC_DEFINE_UNQUOTED([NTL_LIBS],"$NTL_LIBS",[NTL_LIBS])

AC_DEFINE_UNQUOTED([GMP_CPPFLAGS],"$GMP_CPPFLAGS",[GMP_CPPFLAGS])
AC_DEFINE_UNQUOTED([GMP_LIBS],"$GMP_LIBS",[GMP_LIBS])

# AC_DEFINE_UNQUOTED([FLINT_CFLAGS],"$FLINT_CFLAGS",[FLINT_CFLAGS])
# AC_DEFINE_UNQUOTED([FLINT_LIBS],"$FLINT_LIBS",[FLINT_LIBS])

## AC_DEFINE([HAVE_POLYEXTENSIONS], [1], [Enable the algebraic & transcendental extensions])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([misc/Makefile])
AC_CONFIG_FILES([reporter/Makefile])
AC_CONFIG_FILES([coeffs/Makefile])
AC_CONFIG_FILES([polys/Makefile])
AC_CONFIG_FILES([tests/Makefile])
AC_CONFIG_FILES([libpolys-config], [chmod +x libpolys-config])
AC_CONFIG_FILES([libpolys.pc])
AC_OUTPUT
