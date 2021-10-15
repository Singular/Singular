# Check whether the compiler accepts -pipe

dnl SING_CHECK_PIPE
dnl
dnl check whether the compiler accepts -pipe and define PIPE

AC_DEFUN([SING_CHECK_PIPE],
[

if test x$ac_cv_c_cpp = xgcc2 -o  x$ac_cv_c_cpp = xgcc3
then
  AC_MSG_CHECKING(whether compiler accepts -pipe)
  temp_cflags="${CFLAGS}"
  CFLAGS="${CFLAGS} -pipe"
  AC_CACHE_VAL(ac_cv_cxx_have_pipe,
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],[ac_cv_cxx_have_pipe=yes],[ac_cv_cxx_have_pipe=no]))
  AC_MSG_RESULT(${ac_cv_cxx_have_pipe})
  CFLAGS=${temp_cflags}
else
  ac_cv_cxx_have_pipe="no"
fi

if test "${ac_cv_cxx_have_pipe}" != yes; then
  PIPE=
else
  PIPE="-pipe"
fi
AC_SUBST(PIPE)
])
