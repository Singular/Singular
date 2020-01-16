AC_DEFUN([LB_CHECK_MATHICGB],
[
#
 AC_ARG_WITH(mathicgb,
   [AS_HELP_STRING(
     [--with-mathicgb=yes|no],
     [Use the MathicGB library. Default is no.]
   )],
   [],
   [with_mathicgb="no"]
 )
 #
 # MathicGB
 dnl Checking these pre-requisites and adding them to libs is necessary
 dnl for some reason, at least on Cygwin.
 BACKUP_LIBS=${LIBS}
 AS_IF( [test "x$with_mathicgb" = xyes],
 [
  AC_LANG_PUSH([C++])
  AC_CHECK_LIB(memtailor, libmemtailorIsPresent, [],
    [AC_MSG_ERROR([Cannot find libmemtailor, which is required for MathicGB.])])
  AC_CHECK_LIB(mathic, libmathicIsPresent, [],
    [AC_MSG_ERROR([Cannot find libmathic, which is required for MathicGB.])])
  AC_CHECK_LIB(tbb, libtbbIsPresent, [],
    [])
  AC_CHECK_LIB(mathicgb, libmathicgbIsPresent, [],
    [AC_MSG_ERROR([Cannot find the MathicGB library.])])
  AC_CHECK_HEADER([mathicgb.h])
  AC_LANG_POP([C++])
  MATHIC_LIBS=${LIBS}
  LIBS=${BACKUP_LIBS}
  AC_SUBST(MATHIC_LIBS)
  AC_DEFINE(HAVE_MATHICGB,1,[Define if mathicgb is to be used])
#  AC_SUBST(HAVE_MATHICGB_VALUE, 1)
 ])
 #
])
