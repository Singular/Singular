AC_DEFUN([SING_CHECK_PLURAL],
[
AC_MSG_CHECKING(whether non-commutative subsystem should be enabled)

AC_ARG_ENABLE(plural,
[  --enable-plural Enable non-commutative subsystem
],
[if test $enableval = yes; then
     ENABLE_PLURAL="yes"
 else
     ENABLE_PLURAL="no"
 fi
],[ENABLE_PLURAL="yes"])

if test x$ENABLE_PLURAL = xyes; then
  AC_MSG_NOTICE([PLURAL is enabled])
  AC_DEFINE(HAVE_PLURAL,1,Enable non-commutative subsystem)
fi

AM_CONDITIONAL([ENABLE_PLURAL],[test x$ENABLE_PLURAL = xyes])
])
