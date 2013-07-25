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
 AC_CHECK_LIB(cddgmp,dd_set_global_constants,[
  CDDGMPLDFLAGS="-lcddgmp"
  PASSED_ALL_TESTS_FOR_GFANLIB="yes";
  ],[
  PASSED_ALL_TESTS_FOR_GFANLIB="no";
  echo "Error! cddgmp needed!"
  exit -1
  ])
else
 AC_MSG_RESULT(no)
 PASSED_ALL_TESTS_FOR_GFANLIB="no";
fi

AM_CONDITIONAL(HAVE_GFANLIB, test "x$PASSED_ALL_TESTS_FOR_GFANLIB" = xyes)

])
