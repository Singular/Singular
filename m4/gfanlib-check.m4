# gfanlib.m4

AC_DEFUN([SING_CHECK_GFANLIB],
[

AC_ARG_ENABLE(gfanlib,
AS_HELP_STRING([--enable-gfanlib], [Enables gfanlib, a package for basic convex geometry]),
[ENABLE_GFANLIB="yes"],
[ENABLE_GFANLIB="no"])

AC_MSG_CHECKING(whether to build with gfanlib)

if test "x$ENABLE_GFANLIB" = xyes; then
 AC_MSG_RESULT(yes)

 AC_CHECK_HEADERS([setoper.h cdd/setoper.h cddlib/setoper.h])

 if test "$ac_cv_header_setoper_h" == no -a "$ac_cv_header_cdd_setoper_h" == no -a "$ac_cv_header_cddlib_setoper_h" == no
 then
	AC_MSG_WARN([Error, setoper.h is missing!]) 
 fi

 AC_MSG_CHECKING([whether libcddgmp is usable])

 BACKUP_LIBS=$LIBS

 LIBS="$LIBS -lcddgmp $GMP_LIBS "

 AC_LANG_PUSH(C)
 AC_LINK_IFELSE(
  [
   AC_LANG_PROGRAM(
    [
    #define GMPRATIONAL
     #ifdef HAVE_SETOPER_H
     # include <setoper.h>
     # include <cdd.h>
     #endif
     #ifdef HAVE_CDD_SETOPER_H
     # include <cdd/setoper.h>
     # include <cdd/cdd.h>
     #endif
     #ifdef HAVE_CDDLIB_SETOPER_H
     # include <cddlib/setoper.h>
     # include <cddlib/cdd.h>
     #endif
    ], [dd_set_global_constants(); dd_log=dd_FALSE; ] 
    )
  ],
  [PASSED_ALL_TESTS_FOR_GFANLIB="1"] [CDDGMPLDFLAGS="-lcddgmp $GMP_LIBS"]  [CDDGMPCPPFLAGS="-DGMPRATIONAL"],
  [PASSED_ALL_TESTS_FOR_GFANLIB="0"]
 )
 AC_LANG_POP()

 LIBS=$BACKUP_LIBS

else
 PASSED_ALL_TESTS_FOR_GFANLIB="0"
fi

 if test "x$PASSED_ALL_TESTS_FOR_GFANLIB" = x1; then  
  AC_MSG_RESULT(yes)
  AC_SUBST(CDDGMPLDFLAGS) 
  AC_SUBST(CDDGMPCPPFLAGS) 
 else
  AC_MSG_RESULT(no) 
 fi

AM_CONDITIONAL(HAVE_GFANLIB, test "x$PASSED_ALL_TESTS_FOR_GFANLIB" = x1)
AC_DEFINE_UNQUOTED(HAVE_GFANLIB, ${PASSED_ALL_TESTS_FOR_GFANLIB}, [whether gfanlib support is enabled])

])
