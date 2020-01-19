# gfanlib.m4

AC_DEFUN([SING_CHECK_GFANLIB],
[

AC_ARG_ENABLE(gfanlib,
AS_HELP_STRING([--enable-gfanlib], [Enables gfanlib, a package for basic convex geometry]),
[ENABLE_GFANLIB="$enableval"],
[ENABLE_GFANLIB=""])

AC_MSG_CHECKING(whether to check for gfanlib)

if test "x$ENABLE_GFANLIB" != "xno"; then
 AC_MSG_RESULT([yes])

 AC_CHECK_HEADERS([setoper.h cdd/setoper.h cddlib/setoper.h])

 if test "x$ac_cv_header_setoper_h" = xno -a "x$ac_cv_header_cdd_setoper_h" = xno -a "x$ac_cv_header_cddlib_setoper_h" = xno
 then
	AC_MSG_WARN([Error, setoper.h is missing!])
 fi

 AC_MSG_CHECKING([whether libcddgmp is usable])

 BACKUP_LIBS=$LIBS

 LIBS="-lcddgmp -lgmp $GMP_LIBS $LIBS"

 AC_LANG_PUSH(C++)
 AC_LINK_IFELSE(
  [
   AC_LANG_PROGRAM(
    [
    #define GMPRATIONAL
     #ifdef HAVE_SETOPER_H
     # include <setoper.h>
     # include <cdd.h>
     #else
     #ifdef HAVE_CDD_SETOPER_H
     # include <cdd/setoper.h>
     # include <cdd/cdd.h>
     #else
     #ifdef HAVE_CDDLIB_SETOPER_H
     # include <cddlib/setoper.h>
     # include <cddlib/cdd.h>
     #endif
     #endif
     #endif
    ], [dd_set_global_constants(); dd_log=dd_FALSE; ]
    )
  ],
  [PASSED_ALL_TESTS_FOR_GFANLIB="1"] [CDDGMPLDFLAGS="-lcddgmp $GMP_LIBS"]  [CDDGMPCPPFLAGS="-DGMPRATIONAL"],
  [PASSED_ALL_TESTS_FOR_GFANLIB="0"]
 )
 AC_LANG_POP()

 AC_CHECK_FUNCS(dd_free_global_constants)

 LIBS=$BACKUP_LIBS

 if test "x$PASSED_ALL_TESTS_FOR_GFANLIB" = x1; then
  AC_MSG_RESULT([yes])
  AC_SUBST(CDDGMPLDFLAGS)
  AC_SUBST(CDDGMPCPPFLAGS)
 else
  AC_MSG_RESULT([no])
  if test "x$ENABLE_GFANLIB" = "xyes"; then
   AC_MSG_ERROR([Error, could not use libcddgmp])
  fi
 fi
 AC_MSG_RESULT(no)

 AC_SUBST(CDDGMPLDFLAGS)
else
 AC_MSG_RESULT(no)
 PASSED_ALL_TESTS_FOR_GFANLIB="0"
fi



AM_CONDITIONAL(HAVE_GFANLIB, test "x$PASSED_ALL_TESTS_FOR_GFANLIB" = x1)
AC_DEFINE_UNQUOTED(HAVE_GFANLIB, ${PASSED_ALL_TESTS_FOR_GFANLIB}, [whether gfanlib support is enabled])

])
