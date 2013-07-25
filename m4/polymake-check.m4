# polymake.m4

AC_DEFUN([SING_CHECK_POLYMAKE],
[

AC_ARG_ENABLE(polymake,
AS_HELP_STRING([--enable-polymake], [Enables interface for Singular to Polymake (needs gfanlib)]),
[ENABLE_POLYMAKE="yes"],
[ENABLE_POLYMAKE="no"])

AC_MSG_CHECKING(whether to build with polymake interface)

if test "x$ENABLE_POLYMAKE" = xyes; then

  AC_MSG_RESULT([yes])
  if test "x$ENABLE_GFANLIB" != xyes; then
    PASSED_ALL_TEST_FOR_POLYMAKE="no";
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([gfanlib is not enabled])
  else

##  AC_MSG_CHECKING(whether polymake is properly installed)
   AC_CHECK_PROG([PMCONFIG],[polymake-config],[1],[0])
   if test $PMCONFIG = "1"; then
##    AC_MSG_CHECKING([whether polymake is up-to-date])
    SUPPORTEDPOLYMAKEVERSION="212"
    CURRENTPOLYMAKEVERSION=`polymake-config --version | cut -c -4 -| sed s'/\.//'`
    if test $CURRENTPOLYMAKEVERSION -ge $SUPPORTEDPOLYMAKEVERSION; then
      PASSED_ALL_TEST_FOR_POLYMAKE="yes";
      AC_MSG_RESULT([yes])

      PM_INC=`polymake-config --includes`
      PM_CFLAGS=`polymake-config --cflags`
      PM_LIBS=`polymake-config --libs`
      PM_LDFLAGS=`polymake-config --ldflags`

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

    else
      PASSED_ALL_TEST_FOR_POLYMAKE="no";
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([outdated polymake version])
    fi
   else
    PASSED_ALL_TEST_FOR_POLYMAKE="no";
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([polymake not installed])
   fi
  fi
else
  PASSED_ALL_TEST_FOR_POLYMAKE="no";
  AC_MSG_RESULT(no)
fi

AM_CONDITIONAL(SING_HAVE_POLYMAKE, test "x$PASSED_ALL_TEST_FOR_POLYMAKE" = xyes)

])
