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
[  --with-flint=<path>|yes|no  Use FLINT library. If argument is no, you do not have
                            the library installed on your machine (set as
			    default). If argument is yes or <empty> that means
			    the library is reachable with the standard search
			    path (/usr or /usr/local). Otherwise you give the
			    <path> to the directory which contain the library.
	     ],
	     [if test "x$withval" = xyes ; then
			FLINT_HOME_PATH="${DEFAULT_CHECKING_PATH}"
	      elif test "x$withval" != xno ; then
			FLINT_HOME_PATH="$withval"
	     fi],
	     [FLINT_HOME_PATH="${DEFAULT_CHECKING_PATH}"])

dnl Check for existence
BACKUP_CFLAGS=${CFLAGS}
BACKUP_LIBS=${LIBS}

AC_LANG_PUSH([C])

flint_found="no"
dnl check for system installed libraries if FLINT_HOME_PATH is the default
if test "$FLINT_HOME_PATH" = "$DEFAULT_CHECKING_PATH" ; then
	FLINT_CFLAGS=""
	FLINT_LIBS="-lflint -lmpfr -lgmp"

	# we suppose that mpfr and mpir to be in the same place or available by default
	CFLAGS=" ${GMP_CPPFLAGS} ${BACKUP_CFLAGS}"
	LIBS="${FLINT_LIBS} ${GMP_LIBS} ${BACKUP_LIBS}"

        AC_TRY_LINK([#include <flint/fmpz.h>
                    ],
                    [fmpz_t x; fmpz_init(x);], [
                flint_found="yes"
        ])
fi

dnl if flint was not previously found, search FLINT_HOME_PATH
if test "x$flint_found" = "xno" ; then
	for FLINT_HOME in ${FLINT_HOME_PATH}
	do

		FLINT_CFLAGS="-I${FLINT_HOME}/include/"
		FLINT_LIBS="-L${FLINT_HOME}/lib -Wl,-rpath,${FLINT_HOME}/lib -lflint -lmpfr -lgmp"

	# we suppose that mpfr and mpir to be in the same place or available by default
		CFLAGS="${FLINT_CFLAGS} ${GMP_CPPFLAGS} ${BACKUP_CFLAGS}"
		LIBS="${FLINT_LIBS} ${GMP_LIBS} ${BACKUP_LIBS}"

                AC_TRY_LINK([#include <flint/fmpz.h>
                            ],
                            [fmpz_t x; fmpz_init(x);], [
                        flint_found="yes"
                        break
                ])
	done
fi

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
AC_SUBST(FLINT_HOME)
])

