AC_INIT([singular],[3.1.3.sw])
AC_DEFINE([VERSION_DATE],["Jun 2013"],[release date])

_AC_SRCDIRS(["$ac_dir"])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.])

AC_CONFIG_SRCDIR([Singular/tesths.cc])

AC_CONFIG_HEADER([config.h])

AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects]) # -Wno-extra-portability -Werror silent-rules

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])

dnl Check if build env is sane
AM_SANITY_CHECK

SING_RESET_FLAGS()
SING_CHECK_SET_ARGS()

#!
# AC_PROG_CC
# AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CXXCPP
AM_PROG_CC_C_O
### AM_PROG_LEX
AC_PROG_LN_S
AC_PROG_INSTALL
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h iostream.h)

AC_CHECK_FUNCS(readlink getcwd getwd setenv putenv)


AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi 
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
                                                                    
LT_INIT

# Checks for libraries.
AC_CHECK_LIB(rt,clock_gettime)
AC_CHECK_LIB(pthread,pthread_create)

LB_CHECK_GMP(3.1.1,,AC_MSG_ERROR([Unable to find GMP on your machine: please use --with-gmp=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))
LB_CHECK_NTL(5.0,,AC_MSG_WARN([Unable to find NTL (which is strongly recommended) on your machine: please use --with-ntl=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))
LB_CHECK_FLINT(2.3,,AC_MSG_WARN([Unable to find FLINT (which is strongly recommended) on your machine: please use --with-flint=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

AX_PYTHON_DEFAULT()
AX_PYTHON_WITH_VERSION([2.4])

AC_CONFIG_GOOGLE_PERFTOOLS()

AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC

# SING_CHECK_PIPE

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
SING_CHECK_GFANLIB

### the following is needed due to the use of om_sing_opt_show_mem in misc_ip.cc...
#ac_configure_args="$ac_configure_args --with-external-config_h=../Singular/omSingularConfig.h"

AC_ARG_ENABLE(omalloc, AS_HELP_STRING([--disable-omalloc], [do NOT use omalloc within the factory]),
 [if test "x$enableval"  = "xyes"; then
   ENABLE_OMALLOC=yes
 fi], ENABLE_OMALLOC=add)

ENABLE_OMALLOC_ARG="--with-omalloc-dir=$ac_abs_top_srcdir --with-omalloc-extra-dir=$ac_abs_top_builddir --enable-omalloc-trust"

if test "x$ENABLE_OMALLOC" = xadd; then
   ENABLE_OMALLOC=yes
   ENABLE_OMALLOC_ARG="--enable-omalloc $ENABLE_OMALLOC_ARG"
fi

if test "x$ENABLE_FACTORY" = xyes; then
 AC_MSG_CHECKING(whether to use omalloc in factory)
 if test "x$ENABLE_OMALLOC" = xyes; then
  AC_MSG_RESULT(yes)
  ac_configure_args="$ac_configure_args $ENABLE_OMALLOC_ARG"
 else
  AC_MSG_RESULT(no)
 fi
fi

AC_ARG_ENABLE(countedref, AS_HELP_STRING([--enable-countedref], [Enable autoloading of reference counted types]),
[if test "x$enableval" = "xyes"; then
ENABLE_COUNTEDREF_AUTOLOAD=yes
fi], ENABLE_COUNTEDREF_AUTOLOAD=no)

if test x"${ENABLE_COUNTEDREF_AUTOLOAD}" == xyes; then
  AC_DEFINE([SI_COUNTEDREF_AUTOLOAD],1,[Enable autoloading of reference counted types])
  AC_SUBST(SI_COUNTEDREF_AUTOLOAD)
fi

dnl AC_CONFIG_FILES conditionalization requires using AM_COND_IF, however
dnl AM_COND_IF is new to Automake 1.11.  To use it on new Automake without
dnl requiring same, a fallback implementation for older Autoconf is provided.
dnl Note that disabling of AC_CONFIG_FILES requires Automake 1.11, this code
dnl is correct only in terms of m4sh generated script.
m4_ifndef([AM_COND_IF], [AC_DEFUN([AM_COND_IF], [
if test -z "$$1_TRUE"; then :
  m4_n([$2])[]dnl
m4_ifval([$3],
[else
    $3
])dnl
fi[]dnl
])])


AC_DEFINE_UNQUOTED([CC],"$CC",[CC])
AC_DEFINE_UNQUOTED([CXX],"$CXX",[CXX])
AC_DEFINE_UNQUOTED([CFLAGS],"$CFLAGS",[CFLAGS])
AC_DEFINE_UNQUOTED([CXXFLAGS],"$CXXFLAGS",[CXXFLAGS])
AC_DEFINE_UNQUOTED([DEFS],"$DEFS",[DEFS])
AC_DEFINE_UNQUOTED([CPPFLAGS],"$CPPFLAGS",[CPPFLAGS])
AC_DEFINE_UNQUOTED([LDFLAGS],"$LDFLAGS",[LDFLAGS])
AC_DEFINE_UNQUOTED([LIBS],"$LIBS",[LIBS])
 
  
SING_SHOW_FLAGS([Compiler/linker flags: ])

    
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([dox/Doxyfile])])
AC_CONFIG_FILES([dox/Makefile])

AC_CONFIG_SUBDIRS([resources])
AC_CONFIG_SUBDIRS([omalloc])

if test "x$ENABLE_FACTORY" = xyes; then
 AC_CONFIG_SUBDIRS([factory])
fi

AC_CONFIG_SUBDIRS([libpolys])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([xalloc/Makefile])
AC_CONFIG_FILES([kernel/Makefile])
AC_CONFIG_FILES([numeric/Makefile])
AC_CONFIG_FILES([Singular/Makefile])
AC_CONFIG_FILES([dyn_modules/Makefile])
AC_CONFIG_FILES([dyn_modules/bigintm/Makefile])
AC_CONFIG_FILES([dyn_modules/syzextra/Makefile])
AC_CONFIG_FILES([dyn_modules/callgfanlib/Makefile])
AC_CONFIG_FILES([IntegerProgramming/Makefile])
AC_CONFIG_FILES([libsingular-config])

AC_CONFIG_SUBDIRS([gfanlib])

AC_CONFIG_FILES([redhat/singular.spec])
# AC_CONFIG_FILES([desktop/Singular.desktop])
# AC_CONFIG_FILES([desktop/Singular-manual.desktop])

AC_CONFIG_FILES([redhat/Makefile])
AC_CONFIG_FILES([emacs/Makefile])
AC_CONFIG_FILES([debian/Makefile])
AC_CONFIG_FILES([desktop/Makefile])

AC_OUTPUT
