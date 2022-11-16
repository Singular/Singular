# Check for FLINT
# Bradford Hovinen, 2001-06-13
# Modified by Pascal Giorgi, 2003-12-03
# Inspired by gnome-bonobo-check.m4 by Miguel de Icaza, 99-04-12
# Stolen from Chris Lahey       99-2-5
# stolen from Manish Singh again
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl Test for FLINT and define
dnl FLINT_CFLAGS and FLINT_LIBS

AC_DEFUN([LB_CHECK_FLINT],
[
AC_REQUIRE([SING_DEFAULT_CHECKING_PATH])

AC_ARG_WITH(flint,
[  --with-flint=<path>|yes|no  Use FLINT library. By default (yes), Singular will
                            try to find the library and use it if found.
                            If argument is no, Singular will not use the
                            library even if it could be found on your machine.
                            Otherwise you can give the <path> to the directory
                            which contains the library.
             ],
             [if test "x$withval" = xyes ; then
                        FLINT_HOME_PATH="DEFAULTS ${DEFAULT_CHECKING_PATH}"
              elif test "x$withval" != xno ; then
                        FLINT_HOME_PATH="$withval"
             fi],
             [FLINT_HOME_PATH="DEFAULTS ${DEFAULT_CHECKING_PATH}"])

dnl Check for existence
BACKUP_CFLAGS=${CFLAGS}
BACKUP_LIBS=${LIBS}

AC_LANG_PUSH([C])

flint_found="no"
dnl if flint was not previously found, search FLINT_HOME_PATH
for FLINT_HOME in ${FLINT_HOME_PATH}
do
        if test "$FLINT_HOME" = DEFAULTS; then
                FLINT_CFLAGS=""
                FLINT_LIBS="-lflint -lmpfr -lgmp"
        else
                FLINT_CFLAGS="-I${FLINT_HOME}/include/"
                FLINT_LIBS="-L${FLINT_HOME}/lib -Wl,-rpath,${FLINT_HOME}/lib -lflint -lmpfr -lgmp"
        fi

        # we suppose that mpfr and mpir to be in the same place or available by default
                CFLAGS="${FLINT_CFLAGS} ${GMP_CPPFLAGS} ${BACKUP_CFLAGS}"
                LIBS="${FLINT_LIBS} ${GMP_LIBS} ${BACKUP_LIBS}"

                AC_LINK_IFELSE(
                [AC_LANG_PROGRAM([[#include <flint/fmpz.h>]],
                            [[fmpz_t x; fmpz_init(x);]])],
                        [flint_found="yes"],
                        [break])
done

AC_LANG_POP([C])

CFLAGS=${BACKUP_CFLAGS}
LIBS=${BACKUP_LIBS}
#unset LD_LIBRARY_PATH

AC_MSG_CHECKING(for FLINT)

if test "x$flint_found" = "xyes" ; then
        AC_DEFINE(HAVE_FLINT,1,[Define if FLINT is installed])
        HAVE_FLINT=yes
        AC_MSG_RESULT(found)
else
        AC_MSG_RESULT(not found)
        FLINT_CFLAGS=""
        FLINT_LIBS=""
        FLINT_HOME=""
fi
AC_SUBST(FLINT_CFLAGS)
AC_SUBST(FLINT_LIBS)
])

