AC_INIT([singular], [3.1.2.sw])

AC_CONFIG_AUX_DIR([.])

AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_RANLIB

SING_CHECK_FACTORY
# Checks for libraries.
LB_CHECK_GMP(3.1.1,,AC_MSG_ERROR([Unable to find GMP on your machine: please use --with-gmp=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))
LB_CHECK_NTL(5.0,,AC_MSG_WARN([Unable to find NTL (which is strongly recommended) on your machine: please use --with-ntl=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))


### the following is needed due to the use of om_sing_opt_show_mem in misc_ip.cc...
#ac_configure_args="$ac_configure_args --with-external-config_h=../Singular/omSingularConfig.h"

AC_CONFIG_SUBDIRS(omalloc)

if test x$ENABLE_FACTORY = xyes; then
  AC_CONFIG_SUBDIRS(factory)
fi

AC_CONFIG_SUBDIRS(libpolys)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([numeric/Makefile])

AC_OUTPUT
