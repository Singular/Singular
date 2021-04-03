AC_DEFUN([SING_CHECK_GMP], [
# Check whether --with-gmp was given.
AC_ARG_WITH([gmp],[AS_HELP_STRING([--with-gmp=path],
                    [provide a non-standard location of gmp])])
AC_REQUIRE([SING_DEFAULT_CHECKING_PATH])
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

CFLAGS="${BACKUP_CFLAGS} ${GMP_CPPFLAGS}"
LIBS=" ${GMP_LIBS} ${BACKUP_LIBS}"

AC_SUBST(GMP_CPPFLAGS)
AC_SUBST(GMP_LIBS)

AC_CHECK_HEADERS([gmp.h], ,[AC_MSG_ERROR([GNU MP not found])])
AC_CHECK_LIB(gmp, __gmpz_init, , [AC_MSG_ERROR([GNU MP not found])])

])
