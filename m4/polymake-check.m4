# polymake.m4

AC_DEFUN([SING_CHECK_POLYMAKE],
[

AC_ARG_ENABLE(polymake,
 AS_HELP_STRING([--enable-polymake], [Enables interface for Singular to Polymake (needs gfanlib)]),
 [ENABLE_POLYMAKE="$enableval"], [ENABLE_POLYMAKE=""])

AC_MSG_CHECKING(whether to check for polymake interface)

if test "x$ENABLE_POLYMAKE" != xno; then
  AC_MSG_RESULT([yes])


  if test "x$PASSED_ALL_TESTS_FOR_GFANLIB" != x1; then

   PASSED_ALL_TEST_FOR_POLYMAKE="no";

   if test "x$ENABLE_POLYMAKE" = xyes; then
    AC_MSG_ERROR([gfanlib was not enabled])
   else
    AC_MSG_WARN([gfanlib was not enabled])
   fi

  else

##  AC_MSG_CHECKING(whether polymake is properly installed)
   AC_CHECK_PROG([PMCONFIG],[polymake-config],[yes],[no])
   if test $PMCONFIG = "yes"; then
##    AC_MSG_CHECKING([whether polymake is up-to-date])
    SUPPORTEDPOLYMAKEVERSION="21"
    CURRENTPOLYMAKEVERSION=`polymake-config --version | cut -c -3 -| sed s'/\.//'`
    if test $CURRENTPOLYMAKEVERSION -ge $SUPPORTEDPOLYMAKEVERSION; then
      AC_MSG_RESULT([yes])

      PM_INC=`polymake-config --includes`
      PM_CFLAGS=`polymake-config --cflags`
      PM_LIBS=`polymake-config --libs`
      PM_LDFLAGS=`polymake-config --ldflags`
      POLYMAKEVERSION=`polymake-config --version | cut -c -6 -| sed s'/\./*100+/'|sed s'/\./*10+/'`

      AC_DEFINE_UNQUOTED(POLYMAKEVERSION,$POLYMAKEVERSION,[check the polymake version])
      AC_SUBST(PM_INC)
      AC_SUBST(PM_CFLAGS)
      AC_SUBST(PM_LIBS)
      AC_SUBST(PM_LDFLAGS)

      AC_DEFINE(HAVE_POLYMAKE,1,[Define if POLYMAKE is installed])

      AC_MSG_CHECKING([polymake includes])
      AC_MSG_RESULT($PM_INC)
      AC_MSG_CHECKING([polymake cflags])
      AC_MSG_RESULT($PM_CFLAGS)
      AC_MSG_CHECKING([polymake libs])
      AC_MSG_RESULT($PM_LIBS)
      AC_MSG_CHECKING([polymake ldflags])
      AC_MSG_RESULT($PM_LDFLAGS)

      PASSED_ALL_TEST_FOR_POLYMAKE="yes";
    else
      PASSED_ALL_TEST_FOR_POLYMAKE="no";

      if test "x$ENABLE_POLYMAKE" = xyes; then
        AC_MSG_ERROR([outdated polymake version])
      fi
    fi
   else
    PASSED_ALL_TEST_FOR_POLYMAKE="no";

    if test "x$ENABLE_POLYMAKE" = xyes; then
     AC_MSG_ERROR([polymake not installed])
    fi
   fi
  fi
else
  AC_MSG_RESULT(no)

  PASSED_ALL_TEST_FOR_POLYMAKE="no";
fi

AM_CONDITIONAL(SING_HAVE_POLYMAKE, test "x$PASSED_ALL_TEST_FOR_POLYMAKE" != xno)

])
