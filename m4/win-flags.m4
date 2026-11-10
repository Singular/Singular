AC_DEFUN([SING_WIN_FLAGS], [
dnl To ensure that $enable_shared is defined
  AC_REQUIRE([LT_INIT])

  AC_CANONICAL_HOST

AC_MSG_CHECKING(checking host)
  case $host_os in
    *mingw*) os_win32=yes; native_win32=yes;;
    *cygwin*) os_win32=yes; native_win32=no;;
    *) os_win32=no; native_win32=no;;
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
  AM_CONDITIONAL([SING_NATIVE_WIN], [test x$native_win32 = xyes])
  if test x$native_win32 = xyes; then
    AC_DEFINE([SINGULAR_NATIVE_WINDOWS], [1],
      [Define when building a native Windows binary with MinGW])
    if test x$enable_shared = xno; then
      AC_DEFINE([FACTORY_STATIC], [1],
        [Define when Factory is linked statically on Windows])
      CPPFLAGS="$CPPFLAGS -DFACTORY_STATIC"
    fi
    dnl Singular's legacy immediate-number representation deliberately stores
    dnl 32-bit values in pointer-typed handles.  GCC diagnoses the round trip
    dnl through Win64's 32-bit long as an error unless permissive mode is used.
    CXXFLAGS="$CXXFLAGS -fpermissive"
  fi
])
