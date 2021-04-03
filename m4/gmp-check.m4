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

gmp_found=no
for GMP_HOME in ${GMP_HOME_PATH}
do
    if test "x$GMP_HOME" != "x/usr"; then
      GMP_CPPFLAGS="-I${GMP_HOME}/include"
      GMP_LIBS="-L${GMP_HOME}/lib -Wl,-rpath,${GMP_HOME}/lib -lgmp"
      CFLAGS="${GMP_CPPFLAGS} ${BACKUP_CFLAGS}"
      LIBS=" ${GMP_LIBS} ${BACKUP_LIBS}"
    else
      GMP_CPPFLAGS=""
      GMP_LIBS="-lgmp"
    fi
    AC_CHECK_HEADERS([gmp.h], [
      AC_CHECK_LIB(gmp, __gmpz_init, [
        gmp_found=yes
        break
      ])
    ])
done
if test "$gmp_found" != yes; then
    AC_MSG_ERROR([GNU MP not found])
fi

AC_SUBST(GMP_CPPFLAGS)
AC_SUBST(GMP_LIBS)

])
