# gfanlib.m4

AC_DEFUN([SING_CHECK_GFANLIB],
[

AC_ARG_ENABLE(gfanlib,
AS_HELP_STRING([--enable-gfanlib], [Enables gfanlib, a package for basic convex geometry]),
[ENABLE_GFANLIB="$enableval"],
[ENABLE_GFANLIB=""])

AC_MSG_CHECKING(whether to check for gfanlib)
PASSED_ALL_TESTS_FOR_GFANLIB="0"

if test "x$ENABLE_GFANLIB" = "xno"; then
  AC_MSG_RESULT([no])
else
  AC_MSG_RESULT([yes])

  AC_CHECK_HEADERS([setoper.h cdd/setoper.h cddlib/setoper.h])

  if test "x$ac_cv_header_setoper_h" = xno -a "x$ac_cv_header_cdd_setoper_h" = xno -a "x$ac_cv_header_cddlib_setoper_h" = xno
  then
    if test "x$ENABLE_GFANLIB" = "xyes"; then
      AC_MSG_ERROR([Error, setoper.h is missing!])
    fi
  else

    # Check whether --with-gmp was given.
    AC_REQUIRE([SING_CHECK_GMP])

    AC_LANG_PUSH(C++)
    AC_SEARCH_LIBS([dd_free_global_constants],[cddgmp],
       [PASSED_ALL_TESTS_FOR_GFANLIB="1"
        CDDGMPLDFLAGS="-lcddgmp $GMP_LIBS"
        CDDGMPCPPFLAGS="-DGMPRATIONAL"],
       [if test "x$ENABLE_GFANLIB" = "xyes"; then
         AC_MSG_WARN("could not determine if libcddgmp is usable")
        fi
       ],
       [$GMP_LIBS])
    AC_LANG_POP()

    AC_SUBST(CDDGMPCPPFLAGS)
    AC_SUBST(CDDGMPLDFLAGS)
  fi
fi

AM_CONDITIONAL(HAVE_GFANLIB, test "x$PASSED_ALL_TESTS_FOR_GFANLIB" = x1)
AC_DEFINE_UNQUOTED(HAVE_GFANLIB, ${PASSED_ALL_TESTS_FOR_GFANLIB}, [whether gfanlib support is enabled])

])
