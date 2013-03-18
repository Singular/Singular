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
else
 AC_MSG_RESULT(no)
fi

AM_CONDITIONAL(HAVE_GFANLIB, test "x$ENABLE_GFANLIB" = xyes)

])
