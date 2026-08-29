# gfanlib.m4

AC_DEFUN([SING_CHECK_GFANLIB],
[

AC_REQUIRE([SING_DEFAULT_CHECKING_PATH])

AC_ARG_ENABLE(gfanlib,
AS_HELP_STRING([--enable-gfanlib], [Enables gfanlib, a package for basic convex geometry]),
[ENABLE_GFANLIB="$enableval"],
[ENABLE_GFANLIB=""])

AC_ARG_WITH([cddlib],[AS_HELP_STRING([--with-cddlib=path],
                    [provide a non-standard location of cddlib])], [
    dnl Given
if test "$with_cddlib" = yes ; then
        CDDLIB_HOME_PATH="DEFAULTS ${DEFAULT_CHECKING_PATH}"
        cddlib_requested=yes
elif test "$with_cddlib" != no ; then
        CDDLIB_HOME_PATH="$with_cddlib"
        cddlib_requested=yes
else
        CDDLIB_HOME_PATH=""
        cddlib_requested=no
fi
], [
    dnl Not given
    CDDLIB_HOME_PATH="DEFAULTS ${DEFAULT_CHECKING_PATH}"
    cddlib_requested=no
])

AC_MSG_CHECKING(whether to check for gfanlib)
PASSED_ALL_TESTS_FOR_GFANLIB="0"

if test "x$ENABLE_GFANLIB" = "xno"; then
  AC_MSG_RESULT([no])
else
  AC_MSG_RESULT([yes])

  # Check whether --with-gmp was given.
  AC_REQUIRE([SING_CHECK_GMP])

  BACKUP_CPPFLAGS=${CPPFLAGS}
  BACKUP_LIBS=${LIBS}

  cddlib_found=no
  CDDGMPLDFLAGS=""
  CDDGMPCPPFLAGS=""

  for CDDLIB_HOME in ${CDDLIB_HOME_PATH}
  do
    if test "$CDDLIB_HOME" != "DEFAULTS"; then
      CDDLIB_CPPFLAGS="-I${CDDLIB_HOME}/include"
      CDDLIB_LIBS="-L${CDDLIB_HOME}/lib -Wl,-rpath,${CDDLIB_HOME}/lib -lcddgmp"
    else
      CDDLIB_CPPFLAGS=""
      CDDLIB_LIBS="-lcddgmp"
    fi

    CPPFLAGS="-DGMPRATIONAL ${CDDLIB_CPPFLAGS} ${GMP_CPPFLAGS} ${BACKUP_CPPFLAGS}"
    LIBS="${CDDLIB_LIBS} ${GMP_LIBS} ${BACKUP_LIBS}"

    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([[#include <cddlib/setoper.h>
#include <cddlib/cdd.h>]],
                [[dd_set_global_constants();]])],
         [cddlib_found=yes
          CDDGMPCPPFLAGS="${CDDLIB_CPPFLAGS} -DGMPRATIONAL"
          CDDGMPLDFLAGS="${CDDLIB_LIBS} ${GMP_LIBS}"
          AC_DEFINE([HAVE_CDDLIB_SETOPER_H], [1],
                    [Define if <cddlib/setoper.h> is available])
          break],
         [])

    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([[#include <cdd/setoper.h>
#include <cdd/cdd.h>]],
                [[dd_set_global_constants();]])],
         [cddlib_found=yes
          CDDGMPCPPFLAGS="${CDDLIB_CPPFLAGS} -DGMPRATIONAL"
          CDDGMPLDFLAGS="${CDDLIB_LIBS} ${GMP_LIBS}"
          AC_DEFINE([HAVE_CDD_SETOPER_H], [1],
                    [Define if <cdd/setoper.h> is available])
          break],
         [])

    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([[#include <setoper.h>
#include <cdd.h>]],
                [[dd_set_global_constants();]])],
         [cddlib_found=yes
          CDDGMPCPPFLAGS="${CDDLIB_CPPFLAGS} -DGMPRATIONAL"
          CDDGMPLDFLAGS="${CDDLIB_LIBS} ${GMP_LIBS}"
          break],
         [])
  done

  CPPFLAGS=${BACKUP_CPPFLAGS}
  LIBS=${BACKUP_LIBS}

  if test "x$cddlib_found" = "xyes"; then
    PASSED_ALL_TESTS_FOR_GFANLIB="1"
    AC_SUBST(CDDGMPCPPFLAGS)
    AC_SUBST(CDDGMPLDFLAGS)
  elif test "x$ENABLE_GFANLIB" = "xyes" -o "x$cddlib_requested" = "xyes"; then
    AC_MSG_ERROR([Error, cddlib (headers and libcddgmp) not found!])
  fi
fi

AM_CONDITIONAL(HAVE_GFANLIB, test "x$PASSED_ALL_TESTS_FOR_GFANLIB" = x1)
AC_DEFINE_UNQUOTED(HAVE_GFANLIB, ${PASSED_ALL_TESTS_FOR_GFANLIB}, [whether gfanlib support is enabled])

])
