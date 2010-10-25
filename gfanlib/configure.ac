#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([gfanlib.h])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LN_S
#AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([gmp.h])
AC_CHECK_HEADERS([cdd.h])
AC_CHECK_HEADERS([setoper.h])

if test "$ac_cv_header_gmp_h" == no
then
	AC_MSG_WARN([Error, gmp.h is missing!])
	exit 1
fi

if test "$ac_cv_header_cdd_h" == no
then
	AC_MSG_WARN([Error, cdd.h is missing!])
	exit 1
fi

if test "$ac_cv_header_setoper_h" == no
then
	AC_MSG_WARN([Error, setoper.h is missing!])
	exit 1
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

#AC_MSG_RESULT($ac_cv_singuname)
#if test "$ac_cv_singuname" = unknown; then
#  AC_MSG_WARN(Unknown architecture: Check singuname.sh)
#  ac_cv_singuname="unknown"
#fi
#AC_DEFINE_UNQUOTED(S_UNAME, "$ac_cv_singuname")
#SINGUNAME=$ac_cv_singuname
#AC_SUBST(SINGUNAME)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
