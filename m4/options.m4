AC_DEFUN([SING_CHECK_ARITH_RINGS],
[
AC_MSG_CHECKING(whether arithmetical rings should be enabled)

AC_ARG_ENABLE(arith-rings, AS_HELP_STRING([--disable-arith-rings], [Disable arithmetical rings]),
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

AC_ARG_ENABLE(factory, AS_HELP_STRING([--disable-factory], [Disable factory]),
[if test $enableval = yes; then
     ENABLE_FACTORY="yes"
 else
     ENABLE_FACTORY="no"
 fi
],[ENABLE_FACTORY="yes"])

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

AC_ARG_ENABLE(plural, AS_HELP_STRING([--disable-plural], [Disable non-commutative subsystem]),
[if test "x$enableval" = xyes; then
     ENABLE_PLURAL="yes"
 else
     ENABLE_PLURAL="no"
 fi
],[ENABLE_PLURAL="yes"])

if test "x$ENABLE_PLURAL" = xyes; then
  AC_DEFINE(HAVE_PLURAL,1,Enable non-commutative subsystem)
  #TODO make a seperate switch
  AC_DEFINE(HAVE_SHIFTBBA,1, [Enable letterplace])
fi

AM_CONDITIONAL([ENABLE_PLURAL],[test x$ENABLE_PLURAL = xyes])

AC_MSG_RESULT($ENABLE_PLURAL)


AC_ARG_WITH(RatGB, AS_HELP_STRING([--with-ratGB], [do compile with ratGB support (experimental)]))

AC_MSG_CHECKING(whether to have ratGB)
if test "x$with_ratGB" != xyes && test "x$enable_ratGB" != xyes; then
  AC_MSG_RESULT(no)
else
  AC_DEFINE(HAVE_RATGRING, 1, [Enable RatGB support])
  AC_MSG_RESULT(yes)
fi


])
