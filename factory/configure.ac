
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
AC_INIT([factory], [4.0.0])
AC_CONFIG_SRCDIR(canonicalform.cc)
AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([.])
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
LT_INIT
AC_PROG_MAKE_SET
AC_CHECK_PROG(AR, ar, ar, where-is-your-ar)
AC_CHECK_PROG(M4, m4, m4, where-is-your-m4)
AC_CHECK_PROG(BISON, bison, bison, where-is-your-bison)
AC_CHECK_SIZEOF(long,4)
AC_C_CONST
AC_C_INLINE


#
# - check withs and enables.
#
AC_ARG_WITH(
  Singular,
  [AS_HELP_STRING([--without-Singular],[build NOT for the use with CAS Singular.])],
  ,
  [with_Singular=yes])

AC_ARG_ENABLE(omalloc, AS_HELP_STRING([--enable-omalloc],[build for use with omalloc]),
[if test "x$enableval" = "xyes"; then
  ENABLE_OMALLOC=yes
fi], ENABLE_OMALLOC=no)

AC_ARG_ENABLE(omalloc_trust, AS_HELP_STRING([--enable-omalloc-trust],[do not check the omalloc header]),
[if test "x$enableval" = "xyes"; then
  omalloc_trust=yes
fi], omalloc_trust=no)

AC_ARG_WITH(omalloc_dir,[AS_HELP_STRING([--with-omalloc-dir=PTH],[location of omalloc])], with_omalloc_dir="$withval", with_omalloc_dir="")
AC_ARG_WITH(omalloc_extra_dir,[AS_HELP_STRING([--with-omalloc-extra-dir=PTH],[optional extra location of omalloc])], with_omalloc_extra_dir="$withval", with_omalloc_extra_dir="")

AC_ARG_ENABLE(
  cf-inline,
  [AS_HELP_STRING([--enable-cf-inline],[build Factory with \"configurable inline methods\" enabled.])],
  [enable_cf_inline=$enableval],
  [enable_cf_inline=no])

AC_ARG_ENABLE(
  streamio,
  [AS_HELP_STRING([--enable-streamio],[build Factory with stream IO])],
  ,
  [enable_streamio=no])

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

AH_TEMPLATE([FACTORY_INT64], [Defenition for FACTORY_INT64])
# Always long long int???!
AC_DEFINE([FACTORY_INT64], [long long int]) 

DX_INIT_DOXYGEN($PACKAGE_NAME, MYDOXYGENCONFIG)

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

LB_CHECK_GMP(3.1.1,,AC_MSG_ERROR([Unable to find GMP on your machine: please use --with-gmp=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

LB_CHECK_NTL(5.0,,AC_MSG_WARN([Unable to find NTL (which is strongly recommended) on your machine: please use --with-ntl=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

LB_CHECK_FLINT(2.3,,AC_MSG_WARN([Unable to find FLINT (which is strongly recommended) on your machine: please use --with-flint=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))

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

AC_CHECK_HEADERS(stdio.h stdlib.h string.h time.h math.h, ,
  [ AC_MSG_ERROR(C header files not found) ])
AC_CHECK_HEADERS(cstdio)

if test "x$enable_streamio" != xno; then
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
# - the name of the game and the targets to create.
#
#if test "x$with_Singular" = xyes; then
#  libfactory=libfactory.a
#
#  factorysrc='$(basefactorysrc) $(singfactorysrc)'
#  factoryincl='$(basefactoryincl) $(singfactoryincl)'
#
#  alltargets=cf
#  installtargets=installcf
#  uninstalltargets=uninstallcf
#else
#  libfactory=libcf.a
#
#  factorysrc='$(basefactorysrc)'
#  factoryincl='$(basefactoryincl)'
#
#  alltargets=cf
#  installtargets=installcf
#  uninstalltargets=uninstallcf
#fi

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

AH_TEMPLATE([SINGULAR], [define if linked to Singular])
AH_TEMPLATE([DISABLE_GMP_CPP],[DISABLE_GMP_CPP])
AH_TEMPLATE([NOSTREAMIO],[DISABLE_GMP_CPP])

# Singular
if test "x$with_Singular" = xyes; then
  AC_DEFINE([SINGULAR],[1])  
  AC_DEFINE([DISABLE_GMP_CPP],[1])
  AC_DEFINE([NOSTREAMIO],[1])
fi

# arithmetic shift
#if test "x$ac_cv_shift" = xyes; then   
#  AC_DEFINE([HAS_ARITHMETIC_SHIFT],[1],[HAS_ARITHMETIC_SHIFT])
#fi

AH_TEMPLATE([HAVE_OMALLOC], [define if build with OMALLOC])

AC_MSG_CHECKING([whether to use omalloc/omalloc.h])
if test "x$ENABLE_OMALLOC" = xyes; then
  AC_MSG_RESULT(yes)
  
  if test "x$with_omalloc_dir" != "x"; then
    OMALLOC_CFLAGS="-I${with_omalloc_dir}"
  fi
  if test "x$with_omalloc_extra_dir" != "x"; then
    OMALLOC_CFLAGS="$OMALLOC_CFLAGS -I${with_omalloc_extra_dir}"
    OMALLOC_LDFLAGS="$OMALLOC_LDFLAGS -L${with_omalloc_extra_dir}/omalloc"
  fi
  
  CPPFLAGS_save="$CPPFLAGS"
  # adding $with_omalloc_dir and $with_omalloc_extra_dir to header search path
  CPPFLAGS="$CPPFLAGS ${OMALLOC_CFLAGS}"
  if test "x$omalloc_trust" = xno; then
    AC_CHECK_HEADERS([omalloc/omalloc.h],,AC_MSG_ERROR([You asked for omalloc but it cannot be found with your default header search path]))
  else
    AC_CHECK_HEADERS([omalloc/omalloc.h],,AC_MSG_WARN([trusting the omalloc locations given: ${OMALLOC_CFLAGS}]))
  fi
  CPPFLAGS="$CPPFLAGS_save"

#  OMALLOC_G_LDADD="-lresources_g -lomalloc_g"
  OMALLOC_R_LDADD="-lomalloc"

  AC_DEFINE(HAVE_OMALLOC, 1, [omalloc support])
 
  AC_SUBST(OMALLOC_CFLAGS)
  AC_SUBST(OMALLOC_LDFLAGS)

#  AC_SUBST(OMALLOC_G_LDADD)
  AC_SUBST(OMALLOC_R_LDADD)
else
  AC_MSG_RESULT(no)
fi

if test "x$ENABLE_OMALLOC" = xyes && test "x$with_Singular" = xyes; then
  RESOURCES_LDFLAGS="$RESOURCES_LDFLAGS -L${with_omalloc_dir}/resources -L${with_omalloc_extra_dir}/resources -lresources "
  AC_SUBST(RESOURCES_LDFLAGS)
fi 

AM_CONDITIONAL([ENABLE_OMALLOC],[test "x$ENABLE_OMALLOC" = xyes])


AH_TEMPLATE([CF_USE_INLINE], [define to use "configurable inline methods" (see cf_inline.cc)])
# "configurable inline methods"
if test "x$enable_cf_inline" != xno; then
  AC_DEFINE([CF_USE_INLINE],[1])
fi

AH_TEMPLATE([NOSTREAMIO], [define to build factory without stream IO])
# iostream
if test "x$enable_streamio" = xno; then
  AC_DEFINE([NOSTREAMIO],[1])
#else
#  factorysrc="$factorysrc \$(useiofactorysrc)"
#  factoryincl="$factoryincl \$(useiofactoryincl)"
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

factory_version="4.0.0"
factory_configuration="'$ac_configure_args' in `pwd`"
AC_SUBST(factory_version)

AH_TEMPLATE([FACTORYVERSION], [factory version])
AC_DEFINE_UNQUOTED([FACTORYVERSION], "$factory_version")

AH_TEMPLATE([HAVE_LIBFAC], [have libfac])
AC_DEFINE_UNQUOTED([HAVE_LIBFAC], 1)

AH_TEMPLATE([FACTORYCONFIGURATION], [factory configuration])
AC_DEFINE_UNQUOTED([FACTORYCONFIGURATION], "$factory_configuration")

#AH_TEMPLATE([GFTABLEDIR], [where the gftables live])
#AC_DEFINE_UNQUOTED([GFTABLEDIR], "$explicit_gftabledir")



AM_CONDITIONAL(
    [WITH_PARSER_FOR_CANONICAL_FORM], 
    [test x$with_Singular != xyes && test x$enable_streamio != xno])

AC_CONFIG_FILES([Makefile include/factory/Makefile libfac/Makefile factory.pc]) #  ftest/GNUmakefile
AC_OUTPUT

