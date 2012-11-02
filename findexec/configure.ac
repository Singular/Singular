AC_INIT([libfindexec], [3.1.4.sw])

AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([omFindExec.h])
AC_CONFIG_HEADER([config.h])

AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects]) # -Wno-extra-portability -Werror silent-rules

dnl Check if build env is sane
AM_SANITY_CHECK

#
# - Check for CC and CXX but be careful about CFLAGS.
#
test "${CFLAGS+set}" = set || cflags_expl_set=no
AC_PROG_CC
test "${CXXFLAGS+set}" = set || cxxflags_expl_set=no
AC_PROG_CXX
if test "x$GCC" != xyes && test "x$GXX" != xyes; then
  AC_MSG_WARN(you better use gcc to compile Factory)
else
  if test "x$GXX" = xyes && test "x$cxxflags_expl_set" = xno; then
    AC_MSG_WARN([we use \`' as default for \`CXXFLAGS'
                    (instead of the configure default \`-g -O')])
  fi
  if test "x$GCC" = xyes && test "x$cflags_expl_set" = xno; then
    AC_MSG_WARN([we use \`' as default for \`CFLAGS'
                    (instead of the configure default \`-g -O')])
  fi
fi

AM_PROG_CC_C_O
# AM_PROG_AR

AC_PROG_LN_S
AC_PROG_INSTALL

AC_FUNC_MALLOC
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h pwd.h sys/param.h sys/stat.h sys/types.h])
AC_CHECK_FUNCS(readlink getcwd getwd getpwnam setenv putenv)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Turn off shared libraries during beta-testing, since they
# make the build process take too long.
LT_INIT
# ([shared])
# LT_INIT(dlopen disable-static) # doesn't work on PowerPC!


SING_CHECK_PIPE

AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug], [build the debugging version of the libraries]),
 [if test "x$enableval"  = "xyes"; then
   ENABLE_DEBUG=yes
 fi], ENABLE_DEBUG=no)

AM_CONDITIONAL(WANT_DEBUG, test x"${ENABLE_DEBUG}" == xyes)


# CFLAGS
if test "x$GCC" = xyes && test "x$cflags_expl_set" = xno; then
  CFLAGS=""
fi
  
# CXXFLAGS
if test "x$GXX" = xyes && test "x$cxxflags_expl_set" = xno; then
  CXXFLAGS=""
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
