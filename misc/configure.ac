#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([misc], [3.1.2.sw])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([mylimits.h])
AC_CONFIG_HEADERS([auxiliary.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([limits.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.
AC_MSG_CHECKING(CPU for singular)

# check for cpu properties:
AC_CHECK_SIZEOF(long,4)

# UNAME and PATH
ac_cv_singcpuname=`uname -m`
AC_MSG_RESULT($ac_cv_singcpuname)

if test "$ac_cv_singuname" = ix86-Win; then
  EXEC_EXT=".exe"
  AC_DEFINE(SI_CPU_I386,1,"ix86 under Windows")
  AC_SUBST(SI_CPU_I386)
fi
AC_SUBST(EXEC_EXT)
if test "$ac_cv_singuname" = x86_64-Linux; then
  AC_DEFINE(SI_CPU_X86_64,1,"ix86-64 under GNU/Linux")
  AC_SUBST(SI_CPU_X86_64)
fi
if test "$ac_cv_singuname" = ix86-Linux; then
  AC_DEFINE(SI_CPU_I386,1,"ix86 under GNU/Linux")
  AC_SUBST(SI_CPU_I386)
fi
if test "$ac_cv_singuname" = IA64-Linux; then
  AC_DEFINE(SI_CPU_IA64,1,"ia64 under GNU/Linux")
  AC_SUBST(SI_CPU_IA64)
fi
if test "$ac_cv_singuname" = ppc-Linux; then
  AC_DEFINE(SI_CPU_PPC,1,"PPC under GNU/Linux")
  AC_SUBST(SI_CPU_PPC)
fi
if test "$ac_cv_singuname" = ppcMac-darwin; then
  AC_DEFINE(SI_CPU_PPC,1,"PPC under MacOS")
  AC_SUBST(SI_CPU_PPC)
fi
# test again with CPU names
if test "$ac_cv_singcpuname" = i386; then
  AC_DEFINE(SI_CPU_I386,1,"i386")
  AC_SUBST(SI_CPU_I386)
fi
if test "$ac_cv_singcpuname" = i686; then
  AC_DEFINE(SI_CPU_I386,1,"i686")
  AC_SUBST(SI_CPU_I386)
fi
if test "$ac_cv_singcpuname" = x86_64; then
  AC_DEFINE(SI_CPU_X86_64,1,"x86-64")
  AC_SUBST(SI_CPU_X86_64)
fi
if test "$ac_cv_singcpuname" = ia64; then
  AC_DEFINE(SI_CPU_IA64,1,"ia64")
  AC_SUBST(SI_CPU_IA64)
fi
if test "$ac_cv_singcpuname" = sparc; then
  AC_DEFINE(SI_CPU_SPARC,1,"SPARC")
  AC_SUBST(SI_CPU_SPARC)
fi
if test "$ac_cv_singcpuname" = ppc; then
  AC_DEFINE(SI_CPU_PPC,1,"PPC")
  AC_SUBST(SI_CPU_PPC)
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
