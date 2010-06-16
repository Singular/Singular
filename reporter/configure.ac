#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
AC_CONFIG_SRCDIR([reporter.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h pwd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNC([vsnprintf], [AC_DEFINE([HAVE_VSNPRINTF], [1],
                               [Define if vsnprintf exists.])])

# check whether the compiler accepts -pipe
if test x$ac_cv_c_cpp = xgcc2 -o  x$ac_cv_c_cpp = xgcc3
then
  AC_MSG_CHECKING(whether compiler accepts -pipe)
  temp_cflags="${CFLAGS}"
  CFLAGS="${CFLAGS} -pipe"
  AC_CACHE_VAL(ac_cv_cxx_have_pipe,
  AC_TRY_COMPILE(,,ac_cv_cxx_have_pipe=yes,ac_cv_cxx_have_pipe=no))
  AC_MSG_RESULT(${ac_cv_cxx_have_pipe})
  CFLAGS=${temp_cflags}
else
  ac_cv_cxx_have_pipe="no"
fi

if test "${ac_cv_cxx_have_pipe}" != yes; then
  PIPE=
else
  PIPE="-pipe"
fi
AC_SUBST(PIPE)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
