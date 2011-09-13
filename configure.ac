AC_INIT([singular], [3.1.2.sw])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.])

AC_CONFIG_SRCDIR([Singular/tesths.cc])

AC_CONFIG_HEADER([config.h])

AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE


dnl Check if build env is sane
AM_SANITY_CHECK

LT_INIT(dlopen disable-static)

AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LN_S
AC_PROG_INSTALL
# AC_PROG_RANLIB

# Checks for libraries.
LB_CHECK_GMP(3.1.1,,AC_MSG_ERROR([Unable to find GMP on your machine: please use --with-gmp=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))
LB_CHECK_NTL(5.0,,AC_MSG_WARN([Unable to find NTL (which is strongly recommended) on your machine: please use --with-ntl=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC

SING_CHECK_PIPE

# check for cpu properties
AC_CHECK_SIZEOF(long,4)
SING_CHECK_CPU

#check for host:
AC_CANONICAL_HOST
case $host_os in
  *cygwin* ) LIBPREFIX="-Xlinker -Bdynamic";;
  * ) LIBPREFIX="";;
esac


SING_CHECK_P_PROCS
SING_CHECK_FACTORY

### the following is needed due to the use of om_sing_opt_show_mem in misc_ip.cc...
#ac_configure_args="$ac_configure_args --with-external-config_h=../Singular/omSingularConfig.h"

AC_CONFIG_SUBDIRS(omalloc)

if test x$ENABLE_FACTORY = xyes; then
  AC_CONFIG_SUBDIRS(factory)
fi

AC_CONFIG_SUBDIRS(libpolys)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([kernel/Makefile])
AC_CONFIG_FILES([numeric/Makefile])

AC_OUTPUT
