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
    DEFAULT_CHECKING_PATH="/opt/homebrew /opt/local /sw /usr/local /usr"
    GMP_HOME_PATH="${DEFAULT_CHECKING_PATH}"
    if test "$with_gmp" = yes ; then
      GMP_HOME_PATH="${DEFAULT_CHECKING_PATH}"
    elif test "$with_gmp" != no ; then
      GMP_HOME_PATH="$with_gmp"
    fi

    BACKUP_CFLAGS=${CFLAGS}
    BACKUP_LIBS=${LIBS}

    for GMP_HOME in ${GMP_HOME_PATH}
    do
      if test "x$GMP_HOME" != "x/usr"; then
        if test -e ${GMP_HOME}/include/gmp.h; then
          GMP_CPPFLAGS="-I${GMP_HOME}/include"
          GMP_LIBS="-L${GMP_HOME}/lib -Wl,-rpath -Wl,${GMP_HOME}/lib -lgmp"
          break
        fi
      fi
    done
    if test -z "${GMP_LIBS}"
    then
      GMP_LIBS="-lgmp"
    fi

    AC_LANG_PUSH(C++)
    AC_CHECK_LIB([cddgmp], [dd_free_global_constants],
       [PASSED_ALL_TESTS_FOR_GFANLIB="1"
        CDDGMPLDFLAGS="-lcddgmp $GMP_LIBS"
        CDDGMPCPPFLAGS="-DGMPRATIONAL"],
       [if test "x$ENABLE_GFANLIB" = "xyes"; then
         AC_MSG_ERROR([Error, could not use libcddgmp])
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
