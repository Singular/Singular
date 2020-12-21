AC_DEFUN([SING_WIN_FLAGS], [
dnl To ensure that $enable_shared is defined
  AC_REQUIRE([LT_INIT])

  AC_CANONICAL_HOST

AC_MSG_CHECKING(checking host)
dnl Maybe necessary for mingw as well but haven't tested
  case $host_os in
    *cygwin* | *mingw*) os_win32=yes;;
    *) os_win32=no;;
  esac

  AC_SUBST(SINGULAR_LDFLAGS)

  if test x$os_win32 = xyes; then
    AC_MSG_RESULT(windows)
    AX_APPEND_LINK_FLAGS([-Wl,-Bdynamic])
dnl libtool requires "-no-undefined" for win32 dll
    SINGULAR_LDFLAGS="$SINGULAR_LDFLAGS -no-undefined"
  else
    AC_MSG_RESULT(unix)
  fi

  AM_CONDITIONAL([SING_WIN], [test x$os_win32 = xyes])
 AM_CONDITIONAL([SI_BUILTIN_SUBSETS], [test x$os_win32 = xyes])
 AM_CONDITIONAL([SI_BUILTIN_FREEALGEBRA], [test x$os_win32 = xyes])
 AM_CONDITIONAL([SI_BUILTIN_PARTIALGB], [test x$os_win32 = xyes])
 AM_CONDITIONAL([SI_BUILTIN_SYZEXTRA], [test x$os_win32 = xyes])
 AM_CONDITIONAL([SI_BUILTIN_PYOBJECT], [test x$bi_pyobject = xyes])
 AM_CONDITIONAL([SI_BUILTIN_GFANLIB], [test x$os_win32 = xyes])
 AM_CONDITIONAL([SI_BUILTIN_CUSTOMSTD], [test x$os_win32 = xyes])
 AM_CONDITIONAL([SI_BUILTIN_GITFAN], [test x$os_win32 = xyes])
 AM_CONDITIONAL([SI_BUILTIN_INTERVAL], [test x$os_win32 = xyes])
 AM_CONDITIONAL([SI_BUILTIN_SYSTHREADS], [test x$os_win32 = xyes])
])
