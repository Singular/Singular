# julia.m4

AC_DEFUN([SING_CHECK_JULIA],
[

AC_ARG_ENABLE(julia,
 AS_HELP_STRING([--enable-julia], [Enables interface for Singular to julia]),
 [ENABLE_JULIA="$enableval"], [ENABLE_JULIA="no"])

AC_MSG_CHECKING(whether to check for julia interface)

if test "x$ENABLE_JULIA" != xno; then
  AC_MSG_RESULT([yes])
  AC_DEFINE(HAVE_JULIA,1,[Define if julia is installed])
else
  AC_MSG_RESULT(no)
fi

])
