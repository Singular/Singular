AC_DEFUN([SING_WIN_FLAGS], [
dnl To ensure that $enable_shared is defined
  AC_REQUIRE([LT_INIT])

  AC_CANONICAL_HOST

dnl Maybe necessary for mingw as well but haven't tested
  case $host_os in
    *cygwin* ) AX_APPEND_LINK_FLAGS([-Wl,-Bdynamic]);;
  esac

dnl libtool requires "-no-undefined" for win32 dll
  AC_SUBST(SINGULAR_LDFLAGS)
  case $host_os in
    *cygwin* | *mingw*)
      if test x"$enable_shared" = xyes; then
        SINGULAR_LDFLAGS="$SINGULAR_LDFLAGS -no-undefined"
      fi
    ;;
  esac
])
