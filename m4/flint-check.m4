# Check for FLINT
# Bradford Hovinen, 2001-06-13
# Modified by Pascal Giorgi, 2003-12-03
# Inspired by gnome-bonobo-check.m4 by Miguel de Icaza, 99-04-12
# Stolen from Chris Lahey       99-2-5
# stolen from Manish Singh again
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl LB_CHECK_FLINT ([MINIMUM-VERSION [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl
dnl Test for FLINT and define
dnl FLINT_CFLAGS and FLINT_LIBS

AC_DEFUN([LB_CHECK_FLINT],
[
DEFAULT_CHECKING_PATH="/usr /usr/local /sw /opt/local"

AC_ARG_WITH(flint,
[  --with-flint=<path>|yes|no  Use FLINT library. If argument is no, you do not have
                            the library installed on your machine (set as
			    default). If argument is yes or <empty> that means
			    the library is reachable with the standard search
			    path (/usr or /usr/local). Otherwise you give the
			    <path> to the directory which contain the library.
	     ],
	     [if test "$withval" = yes ; then
			FLINT_HOME_PATH="${DEFAULT_CHECKING_PATH}"
	      elif test "$withval" != no ; then
			FLINT_HOME_PATH="$withval"
	     fi],
	     [FLINT_HOME_PATH="${DEFAULT_CHECKING_PATH}"])

min_flint_version=ifelse([$1], ,2.3,$1)


dnl Check for existence
BACKUP_CFLAGS=${CFLAGS}
BACKUP_LIBS=${LIBS}

if test -n "$FLINT_HOME_PATH"; then
AC_MSG_CHECKING(for FLINT >= $min_flint_version)
fi

AC_LANG_PUSH([C])

for FLINT_HOME in ${FLINT_HOME_PATH}
 do
 if test -r "$FLINT_HOME/include/flint/fmpz.h"; then

	FLINT_CFLAGS="-I${FLINT_HOME}/include/"
	FLINT_LIBS="-L${FLINT_HOME}/lib -lflint -lmpfr"

	# we suppose that mpfr and mpir to be in the same place or available by default
	CFLAGS="${BACKUP_CFLAGS} ${FLINT_CFLAGS} ${GMP_CPPFLAGS}"
	LIBS="${BACKUP_LIBS} ${FLINT_LIBS} ${GMP_LIBS}"

	AC_TRY_LINK(
	[#include <flint/fmpz.h>],
	[fmpz_t a; fmpz_init (a);],
	[
	AC_TRY_RUN(
	[#include <flint/flint.h>
	int main () { if ((int) version[0] < 2) return -1; else return 0; }
	],[
	flint_found="yes"
	break
	],[
	flint_problem="$problem $FLINT_HOME"
	unset FLINT_CFLAGS
	unset FLINT_LIBS
	],[
	flint_found="yes"
	flint_cross="yes"
	break
	])
	],
	[
	flint_found="no"
	flint_checked="$checked $FLINT_HOME"
	unset FLINT_CFLAGS
	unset FLINT_LIBS
	])
else
	flint_found="no"
fi
done
AC_LANG_POP([C])

CFLAGS=${BACKUP_CFLAGS}
LIBS=${BACKUP_LIBS}
#unset LD_LIBRARY_PATH


if test "x$flint_found" = "xyes" ; then
	AC_SUBST(FLINT_CFLAGS)
	AC_SUBST(FLINT_LIBS)
	AC_SUBST(FLINT_HOME)
	AC_DEFINE(HAVE_FLINT,1,[Define if FLINT is installed])
	HAVE_FLINT=yes
	if test "x$flint_cross" != "xyes"; then
		AC_MSG_RESULT(found)
	else
		AC_MSG_RESULT(unknown)
		echo "WARNING: You appear to be cross compiling, so there is no way to determine"
		echo "whether your FLINT version is new enough. I am assuming it is."
	fi
	ifelse([$2], , :, [$2])
elif test -n "$flint_problem"; then
	AC_MSG_RESULT(problem)
	echo "Sorry, your FLINT version is too old. Disabling."
	ifelse([$3], , :, [$3])
elif test   "x$flint_found" = "xno";  then
	AC_MSG_RESULT(not found)
	ifelse([$3], , :, [$3])
fi

AM_CONDITIONAL(SING_HAVE_FLINT, test "x$HAVE_FLINT" = "xyes")
])

