# gfanlib.m4

AC_DEFUN([SING_CHECK_GFANLIB],
[

AC_ARG_ENABLE(gfanlib,
AS_HELP_STRING([--enable-gfanlib], [Enables gfanlib, a package for basic convex geometry]),
[ENABLE_GFANLIB="yes"],
[ENABLE_GFANLIB="no"])

AC_MSG_CHECKING(whether to build with gfanlib)
if test "x$ENABLE_GFANLIB" = xyes; then
 AC_MSG_RESULT(yes)
 AC_LANG_PUSH(C++)
 SAVE_LIBS=$LIBS
 LIBS="-lcddgmp $GMP_LIBS $LIBS"
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([#include <cdd/setoper.h>
  #include <cdd/cdd.h>],
    [dd_set_global_constants()])],
  [LIBS="-lcddgmp $SAVE_LIBS"] [CDDGMPLDFLAGS="-lcddgmp"] 
  [PASSED_ALL_TESTS_FOR_GFANLIB="yes"],
  [AC_MSG_WARN([libcddgmp is not installed.])] 
  [PASSED_ALL_TESTS_FOR_GFANLIB="no"
  ]
)
if test "$PASSED_ALL_TESTS_FOR_GFANLIB" != yes; then
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([#include <setoper.h>
  #include "cdd.h"],
    [dd_set_global_constants()])],
  [LIBS="$SAVE_LIBS -lcddgmp"] [CDDGMPLDFLAGS="-lcddgmp"] 
  [PASSED_ALL_TESTS_FOR_GFANLIB="yes"],
  [AC_MSG_WARN([libcddgmp is not installed.])] 
  [PASSED_ALL_TESTS_FOR_GFANLIB="no"
  ]
)
fi
if test "$PASSED_ALL_TESTS_FOR_GFANLIB" != yes; then
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([#include <cddlib/setoper.h>
  #include "cddlib/cdd.h"],
    [dd_set_global_constants()])],
  [LIBS="$SAVE_LIBS -lcddgmp"] [CDDGMPLDFLAGS="-lcddgmp"] 
  [PASSED_ALL_TESTS_FOR_GFANLIB="yes"],
  [AC_MSG_WARN([libcddgmp is not installed.])] 
  [PASSED_ALL_TESTS_FOR_GFANLIB="no"
  ]
)
fi
  AC_LANG_POP()
else
 AC_MSG_RESULT(no)
 PASSED_ALL_TESTS_FOR_GFANLIB="no";
fi

LIBS=$SAVE_LIBS

AM_CONDITIONAL(HAVE_GFANLIB, test "x$PASSED_ALL_TESTS_FOR_GFANLIB" = xyes)

])
