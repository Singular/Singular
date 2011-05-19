AC_DEFUN([SING_CHECK_ARITH_RINGS],
[
AC_MSG_CHECKING(whether arithmetical rings should be enabled)

AC_ARG_ENABLE(arith-rings,
[  --enable-arith-rings Enable arithmetical rings
],
[if test $enableval = yes; then
     ENABLE_RINGS="yes"
 else
     ENABLE_RINGS="no"
 fi
],[ENABLE_RINGS="yes"])

if test x$ENABLE_RINGS = xyes; then
  AC_DEFINE(HAVE_RINGS,1,Enable arithmetical rings)
fi

AM_CONDITIONAL([ENABLE_RINGS],[test x$ENABLE_RINGS = xyes])
AC_MSG_RESULT($ENABLE_RINGS)
])

AC_DEFUN([SING_CHECK_FACTORY],
[
AC_MSG_CHECKING(whether factory should be enabled)

AC_ARG_ENABLE(factory,
[  --enable-factory Enable factory
],
[if test $enableval = yes; then
     ENABLE_FACTORY="yes"
 else
     ENABLE_FACTORY="no"
 fi
],[ENABLE_FACTORY="no"])

if test x$ENABLE_FACTORY = xyes; then
  FACTORY_CFLAGS=""
  FACTORY_LIBS="-lfactory"

  AC_SUBST(FACTORY_CFLAGS)
  AC_SUBST(FACTORY_LIBS)
  
  AC_DEFINE(HAVE_FACTORY,1,Enable factory)
fi



AM_CONDITIONAL([ENABLE_FACTORY],[test x$ENABLE_FACTORY = xyes])
AC_MSG_RESULT($ENABLE_FACTORY)
])

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
  AC_DEFINE(HAVE_PLURAL,1,Enable non-commutative subsystem)
fi

AM_CONDITIONAL([ENABLE_PLURAL],[test x$ENABLE_PLURAL = xyes])

AC_MSG_RESULT($ENABLE_PLURAL)
])
