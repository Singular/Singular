AC_DEFUN([SING_WIN_FLAGS], [
dnl To ensure that $enable_shared is defined
  AC_REQUIRE([LT_INIT])

  AC_CANONICAL_HOST

dnl Maybe necessary for mingw as well but haven't tested
  case $host_os in
    *cygwin* | *mingw*) os_win32=yes;;
    *) os_win32=no;;
  esac

  AC_SUBST(SINGULAR_LDFLAGS)

  if test x$os_win32 = xyes; then
    AX_APPEND_LINK_FLAGS([-Wl,-Bdynamic])
dnl libtool requires "-no-undefined" for win32 dll
    SINGULAR_LDFLAGS="$SINGULAR_LDFLAGS -no-undefined"
  fi

  AM_CONDITIONAL([SING_WIN], [test x$os_win32 = xyes])
])
