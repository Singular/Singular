
dnl # emacs edit mode for this file is -*- sh -*-

dnl #
dnl # configure.in - process this file with `autoconf' to generate
dnl #	a `configure' script.
dnl #
dnl # See the `INSTALL' file for information on how the `configure'
dnl # script works.
dnl #

#
# - initialisation.
#
AC_INIT([factory], [4.2.0])
AC_CONFIG_SRCDIR(canonicalform.cc)
AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_HEADER([_config.h])

AM_INIT_AUTOMAKE([-Wall foreign subdir-objects]) # -Wno-extra-portability -Werror silent-rules
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

AM_MAINTAINER_MODE([enable])

# Add pre'prefixed config
AX_PREFIX_CONFIG_H([config.h],[],[_config.h])

#
# - check for CC and CXX but be careful about CFLAGS.
#
AC_CHECK_SIZEOF(long,4)
SING_RESET_FLAGS()
SING_CHECK_SET_ARGS()

#AC_PROG_CC
#AC_PROG_CXX
#
# - check for some other programs.
#
AC_PROG_CPP
AC_PROG_CXXCPP
AC_PROG_LN_S
AC_PROG_INSTALL
## AC_DISABLE_STATIC
AC_PROG_YACC
LT_INIT([win32-dll])
AC_PROG_MAKE_SET
AC_CHECK_PROG(AR, ar, ar, where-is-your-ar)
AC_CHECK_PROG(M4, m4, m4, where-is-your-m4)
AC_CHECK_PROG(BISON, bison, bison, where-is-your-bison)
AC_CHECK_SIZEOF(long,4)
AC_C_CONST
AC_C_INLINE


SING_WIN_FLAGS


#
# - check withs and enables.
#
AC_ARG_ENABLE(
  cf-inline,
  [AS_HELP_STRING([--disable-cf-inline],[build Factory with \"configurable inline methods\" disabled.])],
  [enable_cf_inline=$enableval],
  [enable_cf_inline=yes])

AC_ARG_ENABLE(
  assertions,
  [AS_HELP_STRING([--disable-assertions],[build Factory with no assertions])],
  ,
  enable_assertions=yes)

AC_ARG_ENABLE(
  timing,
  [AS_HELP_STRING([--enable-timing],[build Factory so it will print timing information])],
  ,
  enable_timing=no)

AC_ARG_ENABLE(
  debugoutput,
  [AS_HELP_STRING([--enable-debugoutput],[build Factory so it will print debugging information])],
  ,
  enable_debugoutput=no)

AC_ARG_ENABLE(
  streamio,
  [AS_HELP_STRING([--enable-streamio],[build Factory with stream IO])],
  ,
  [enable_streamio=no])

AH_TEMPLATE([NOSTREAMIO], [define to build factory without stream IO])

AC_ARG_WITH(
  Singular,
  [AS_HELP_STRING([--without-Singular],[(obsolete) build NOT for the use with CAS Singular.])],
  ,
  [with_Singular=yes])

PKG_REQUIRE="$PKG_REQUIRE"
AC_SUBST(PKG_REQUIRE)

SING_CHECK_OMALLOC()

AH_TEMPLATE([SINGULAR], [define if linked to Singular])
AH_TEMPLATE([DISABLE_GMP_CPP],[DISABLE_GMP_CPP])

# Singular
if test "x$with_Singular" = xyes; then
  AC_DEFINE([SINGULAR],[1])
  AC_DEFINE([DISABLE_GMP_CPP],[1])
#  AC_DEFINE([NOSTREAMIO],[1])
fi

# || test "x$with_Singular" = xyes
# Use no streamio (C++) if SINGULAR or explicitely disabled
if test "x$enable_streamio" = xno;  then
  AC_DEFINE([NOSTREAMIO],[1])
fi

AM_CONDITIONAL(
    [WITH_PARSER_FOR_CANONICAL_FORM],
    [test "x$enable_streamio" = xyes])

AC_ARG_VAR( [RESOURCES_INCLUDES], [INCLUDES for libsingular_resources] )
AC_ARG_VAR( [RESOURCES_LIBS], [LIBS for libsingular_resources] )

AC_MSG_CHECKING(whether libsingular_resources should be used)
if test "x$with_Singular" = xyes;
then
  AC_MSG_RESULT(yes)

  AC_MSG_CHECKING([  RESOURCES_INCLUDES?..])
  AC_MSG_RESULT(${RESOURCES_INCLUDES:-unset})

  AC_MSG_CHECKING([  RESOURCES_LIBS?..])
  AC_MSG_RESULT(${RESOURCES_LIBS:-unset})

  PKG_REQUIRE="$PKG_REQUIRE singular_resources"
  AC_SUBST(PKG_REQUIRE)
else
  AC_MSG_RESULT(no)
fi

AM_CONDITIONAL([ENABLE_RESOURCES],[test "x$with_Singular" = xyes])


DX_INIT_DOXYGEN($PACKAGE_NAME, doxygen.cfg, $srcdir/docu)

# do not use `MKINSTALLDIRS' and `MAKEHEADER' since there may be
# name clashes with other peoples configure scripts via
# `config.cache'.  Furthermore, we do not use cache at all to
# avoid some nasty problems with our own development environment.
#unset ac_cv_path_FACTORY_MKINSTALLDIRS
#unset ac_cv_path_FACTORY_MAKEHEADER
#save_path="$PATH"
#PATH="$PATH:$srcdir/bin"
#AC_PATH_PROG(FACTORY_MKINSTALLDIRS, mkinstalldirs, -mkdir)
#PATH="$srcdir/bin:$save_path"
#AC_PATH_PROG(FACTORY_MAKEHEADER, makeheader)
#PATH="$save_path"

#
# - expand paths.
#

#AC_MSG_CHECKING(and generating explicit install paths)

## generate Makefile
#save_prefix="$prefix"
#save_exec_prefix="$exec_prefix"
#test "x$prefix" = xNONE && prefix=$ac_default_prefix
#test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
#for dir in prefix exec_prefix bindir sbindir libexecdir \
#	   datadir sysconfdir sharedstatedir localstatedir libdir \
#	   includedir oldincludedir infodir mandir; do
#  eval "dir_value=\$$dir"
#  cat >> ./conftest.mk << EXPLPATHEOT
#$dir=$dir_value
#explicit_$dir:
#	@ echo \$($dir)
#	@ if echo \$($dir) | grep '^/'; \\
#	then echo \$($dir) > ./conftest.dir; \\
#	else echo \`pwd\`/\$($dir) > ./conftest.dir; fi
#EXPLPATHEOT
#done
#prefix="$save_prefix"
#exec_prefix="$save_exec_prefix"

# generate the explicit paths
#make >&5 2>&1 -f ./conftest.mk explicit_datadir
#explicit_datadir=`cat ./conftest.dir`
#make >&5 2>&1 -f ./conftest.mk explicit_libdir
#explicit_libdir=`cat ./conftest.dir`
#make >&5 2>&1 -f ./conftest.mk explicit_includedir
#explicit_includedir=`cat ./conftest.dir`

# clean up
#rm -f ./conftest.mk ./conftest.dir

#AC_MSG_RESULT(done)

#
# - check for libraries.
#
AC_CHECK_LIB(m, atof, , [ AC_MSG_ERROR(libm.a not found) ])

# Check whether --with-gmp was given.
AC_ARG_WITH([gmp],[AS_HELP_STRING([--with-gmp=path],
                    [provide a non-standard location of gmp])])
DEFAULT_CHECKING_PATH="/usr /usr/local /sw /opt/local /opt/homebrew"
if test "${with_gmp+set}" = set; then :
  if test "$with_gmp" = yes ; then
	GMP_HOME_PATH="${DEFAULT_CHECKING_PATH}"
   elif test "$with_gmp" != no ; then
	GMP_HOME_PATH="$with_gmp ${DEFAULT_CHECKING_PATH}"
    fi
else
  GMP_HOME_PATH="${DEFAULT_CHECKING_PATH}"
fi

BACKUP_CFLAGS=${CFLAGS}
BACKUP_LIBS=${LIBS}

for GMP_HOME in ${GMP_HOME_PATH}
do
  if test "x$GMP_HOME" != "x/usr"; then
    if test -e ${GMP_HOME}/include/gmp.h; then
      GMP_CPPFLAGS="-I${GMP_HOME}/include"
      GMP_LIBS="-L${GMP_HOME}/lib -Wl,-rpath -Wl,${GMP_HOME}/lib -lgmp"
    fi
  else
    GMP_CPPFLAGS=""
    GMP_LIBS="-lgmp"
  fi
  CFLAGS="${BACKUP_CFLAGS} ${GMP_CPPFLAGS}"
  LIBS="${BACKUP_LIBS} ${GMP_LIBS}"
done
AC_SUBST(GMP_CPPFLAGS)
AC_SUBST(GMP_LIBS)

AC_CHECK_HEADERS([gmp.h], ,[AC_MSG_ERROR([GNU MP not found])])
AC_CHECK_LIB(gmp, __gmpz_init, , [AC_MSG_ERROR([GNU MP not found])])

#NTL 9.6.4 requires -lpthread/-pthread
# This test for -lpthread etc has to come before AX_PTHREAD,
# because libtool tends to ignore -pthread in linking shared C++-libs
# see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=25460
# (happens with ubuntu 14.04)
AC_SEARCH_LIBS(sem_wait,[rt pthreads pthread],[],[
  AC_MSG_ERROR([sem_wait not found in rt,pthreads,pthread])
])

LB_CHECK_NTL(5.0,,AC_MSG_WARN([Unable to find NTL (which is strongly recommended) on your machine: please use --with-ntl=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

LB_CHECK_FLINT(2.4,,AC_MSG_WARN([Unable to find FLINT (which is strongly recommended) on your machine: please use --with-flint=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

# arithmetic shift
#AC_MSG_CHECKING(whether your compiler does arithmetic shifts)
#AC_CACHE_VAL(ac_cv_shift,
#  [ LDFLAGS="-L$explicit_libdir $LDFLAGS"
#	AC_TRY_RUN(
#      [ int main() { if (-2 >> 1 == -1) return(0); else return(1); } ],
#      ac_cv_shift=yes, ac_cv_shift=no, ac_cv_shift=yes) ])
#if test "x$ac_cv_shift" = xyes; then
#  AC_MSG_RESULT(yes)
#else
#  AC_MSG_RESULT(no)
#fi

#
# - check for header files.
#
AC_LANG_CPLUSPLUS
AC_LANG_PUSH([C++])

AC_CHECK_HEADERS(stdio.h stdlib.h string.h time.h math.h, ,
  [ AC_MSG_ERROR(C header files not found) ])
AC_CHECK_HEADERS(cstdio)

if test "x$enable_streamio" != xno;  then
  AC_CHECK_HEADERS(iostream.h strstream.h fstream.h iostream string fstream)
  AC_CHECK_HEADERS(ctype.h, , [ AC_MSG_ERROR(standard C header files not found) ])

  # since the FTE will compile only with stream IO enabled we will not
  # check for the necessary header files if stream IO is disabled
  AC_CHECK_HEADERS(stdarg.h signal.h errno.h unistd.h, ,
    [ AC_MSG_WARN(C/C++ header files not found.
                    You will not be able to build the Factory Test Environment
                    (though Factory itself should compile)) ])
fi


if test "x$enable_timing" != xno; then
  AC_CHECK_HEADERS(sys/param.h sys/times.h, ,
    [ AC_MSG_ERROR(\`sys/times.h' or \`sys/param.h' not found.  Try
                  \`configure --disable-timing') ])
fi

# font-lock-trick: '

#
# - check for compiler characteristics.
#

# use C to check compiler characteristics instead of C++.  On
# nextstep, checking with C++ may fail.
# AC_LANG_C

# cross-compiling ?!
#AC_C_CROSS
#if test "x$cross_compiling" = xyes; then
#  AC_MSG_WARN([you better specify a cache file to get the values for
#                    cross-compiling right (e.g., call \`configure
#                    --cache-file=yourTarget.cache').  In particular, you
#                    should make sure that your target machine supports
#                    arithmetic shift.])
#fi

# font-lock-trick: '

#
# - paths.
#

# note that Singular has its own mechanism to search the tables,
# hence we do not need to mind it here
#gftabledir='${datadir}/factory/gftables'
#explicit_gftabledir="$explicit_datadir/gftables"

# for installation of the templates
#templatedir='${includedir}/templates'

#
# - set defines and variables according to our tests.
#

## CFLAGS
#if test "x$GCC" = xyes && test "x$cflags_expl_set" = xno; then
#  CFLAGS=""
#fi

# CXXFLAGS
#if test "x$GXX" = xyes && test "x$cxxflags_expl_set" = xno; then
#  CXXFLAGS=""
#
#AC_LANG_SAVE
#AC_LANG_CPLUSPLUS
# check whether CXX accepts -fno-rtti
#AC_MSG_CHECKING(whether gcc accepts -fno-rtti)
#tmp_flags=${CXXFLAGS}
#CXXFLAGS="${CXXFLAGS}"
#AC_CACHE_VAL(ac_cv_cxx_have_rtti,
#AC_TRY_COMPILE(,,ac_cv_cxx_have_rtti=yes,ac_cv_cxx_have_rtti=no)
#)
#AC_MSG_RESULT(${ac_cv_cxx_have_rtti})
#CXXFLAGS=$tmp_flags
#if test "${ac_cv_cxx_have_rtti}" = yes; then
#CXXFLAGS="$CXXFLAGS"
#fi
#AC_MSG_CHECKING(whether gcc accepts -fno-exceptions)
#tmp_flags=${CXXFLAGS}
#CXXFLAGS="${CXXFLAGS} -fno-exceptions"
#AC_CACHE_VAL(ac_cv_cxx_have_exceptions,
#AC_TRY_LINK(,,ac_cv_cxx_have_exceptions=yes,ac_cv_cxx_have_exceptions=no)
#)
#AC_MSG_RESULT(${ac_cv_cxx_have_exceptions})
#CXXFLAGS=$tmp_flags
#if test "${ac_cv_cxx_have_exceptions}" = yes; then
#CXXFLAGS="$CXXFLAGS -fno-exceptions"
#fi
#AC_LANG_RESTORE
#
#fi

# ARFLAGS
#test "${ARFLAGS+set}" = set || ARFLAGS=cr

# M4FLAGS
#test "${M4FLAGS+set}" = set || M4FLAGS=

# arithmetic shift
#if test "x$ac_cv_shift" = xyes; then
#  AC_DEFINE([HAS_ARITHMETIC_SHIFT],[1],[HAS_ARITHMETIC_SHIFT])
#fi

AH_TEMPLATE([CF_USE_INLINE], [define to use "configurable inline methods" (see cf_inline.cc)])
# "configurable inline methods"
if test "x$enable_cf_inline" != xno; then
  AC_DEFINE([CF_USE_INLINE],[1])
fi

AH_TEMPLATE([NOASSERT], [define if you do not want to activate assertions])
# assertions
if test "x$enable_assertions" = xno; then
  AC_DEFINE([NOASSERT],[1])
fi

AH_TEMPLATE([TIMING], [define if you want to activate the timing stuff])
# timing
if test "x$enable_timing" != xno; then
  AC_DEFINE([TIMING],[1])
fi

# debugoutput
AH_TEMPLATE([DEBUGOUTPUT], [define if you want to have debugging output])
if test "x$enable_debugoutput" != xno; then
  AC_DEFINE([DEBUGOUTPUT],[1])
fi

#
# - complete and substitute variables, defines.
#
#AC_SUBST(gftabledir)
#AC_SUBST(templatedir)



# AC_SUBST(libfactory)

#AC_SUBST(ARFLAGS)
#AC_SUBST(M4FLAGS)

AC_SUBST(MAKEHEADERFLAGS)

#AC_SUBST(factorysrc)
#AC_SUBST(factoryincl)

#AC_SUBST(alltargets)
#AC_SUBST(installtargets)
#AC_SUBST(uninstalltargets)

factory_version="4.1.3"
factory_configuration="'$ac_configure_args' in `pwd`"
AC_SUBST(factory_version)

AH_TEMPLATE([FACTORYVERSION], [factory version])
AC_DEFINE_UNQUOTED([FACTORYVERSION], "$factory_version")

AH_TEMPLATE([FACTORYCONFIGURATION], [factory configuration])
AC_DEFINE_UNQUOTED([FACTORYCONFIGURATION], "$factory_configuration")

#AH_TEMPLATE([GFTABLEDIR], [where the gftables live])
#AC_DEFINE_UNQUOTED([GFTABLEDIR], "$explicit_gftabledir")



AC_CONFIG_FILES([Makefile include/factory/Makefile factory.pc]) #  ftest/GNUmakefile
AC_OUTPUT

