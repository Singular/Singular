AC_INIT([singular],[3.1.3.sw])

_AC_SRCDIRS(["$ac_dir"])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.])

AC_CONFIG_SRCDIR([Singular/tesths.cc])

AC_CONFIG_HEADER([config.h])

AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE([-Wall foreign])

dnl Check if build env is sane
AM_SANITY_CHECK

#
# - Check for CC and CXX but be careful about CFLAGS.
#

test "${CFLAGS+set}" = set || cflags_expl_set=no
AC_PROG_CC
test "${CXXFLAGS+set}" = set || cxxflags_expl_set=no
AC_PROG_CXX
if test "x$GCC" != xyes && test "x$GXX" != xyes; then
  AC_MSG_WARN(you better use gcc to compile Factory)
else
  if test "x$GXX" = xyes && test "x$cxxflags_expl_set" = xno; then
    AC_MSG_WARN([we use \`' as default for \`CXXFLAGS'
                    (instead of the configure default \`-g -O')])
  fi
  if test "x$GCC" = xyes && test "x$cflags_expl_set" = xno; then
    AC_MSG_WARN([we use \`' as default for \`CFLAGS'
                    (instead of the configure default \`-g -O')])
  fi
fi


AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h iostream.h)

AC_CHECK_FUNCS(readlink getcwd getwd)


AC_PROG_CPP
AC_PROG_CXXCPP
AM_PROG_CC_C_O
### AM_PROG_LEX
AC_PROG_LN_S
AC_PROG_INSTALL

LT_INIT
   
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
SING_CHECK_READLINE
SING_CHECK_DBM

# CFLAGS
if test "x$GCC" = xyes && test "x$cflags_expl_set" = xno; then
  CFLAGS=""
fi

# CXXFLAGS
if test "x$GXX" = xyes && test "x$cxxflags_expl_set" = xno; then
  CXXFLAGS=""
fi

AC_DEFINE_UNQUOTED([CC],"$CC",[CC])
AC_DEFINE_UNQUOTED([CXX],"$CXX",[CXX])
AC_DEFINE_UNQUOTED([CFLAGS],"$CFLAGS",[CFLAGS])
AC_DEFINE_UNQUOTED([CXXFLAGS],"$CXXFLAGS",[CXXFLAGS])
AC_DEFINE_UNQUOTED([LDFLAGS],"$LDFLAGS",[LDFLAGS])

### the following is needed due to the use of om_sing_opt_show_mem in misc_ip.cc...
#ac_configure_args="$ac_configure_args --with-external-config_h=../Singular/omSingularConfig.h"

AC_ARG_ENABLE(omalloc, AS_HELP_STRING([--enable-omalloc], [use omalloc for the factory]),
[if test "x$enableval"  = "xyes"; then
  enable_omalloc=yes
fi], enable_omalloc=no)

AC_MSG_CHECKING(whether to use omalloc in factory)
if test "$enable_omalloc" = yes; then

  AC_MSG_RESULT(yes)
  ac_configure_args="$ac_configure_args --with-omalloc-dir=$ac_abs_top_srcdir --with-omalloc-extra-dir=$ac_abs_top_builddir --enable-omalloc-trust"
else
  AC_MSG_RESULT(no)
fi

AC_CONFIG_SUBDIRS(omalloc)

if test x$ENABLE_FACTORY = xyes; then
  AC_CONFIG_SUBDIRS(factory)
fi

AC_CONFIG_SUBDIRS(libpolys)
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([findexec/Makefile])
AC_CONFIG_FILES([kernel/Makefile])
AC_CONFIG_FILES([numeric/Makefile])
AC_CONFIG_FILES([Singular/Makefile])
AC_CONFIG_FILES([IntegerProgramming/Makefile])
AC_CONFIG_FILES([libsingular-config])

AC_OUTPUT
