AC_DEFUN([SING_CHECK_DBM],
[

AC_ARG_WITH(
  dbm,
  [  --without-dbm           do not use dbm (no DBM links)])

AC_MSG_CHECKING(whether to have dbm links)

if test "$with_dbm" != no; then
  AC_DEFINE([HAVE_DBM], [1], [Define to have dbm links])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

])
