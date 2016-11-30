dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl File: configure.ac
dnl Purpose: Process this file with autoconf to produce configure
dnl Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
dnl Created: 11/99
dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll

AC_INIT([xmalloc],[4.0])

AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_SRCDIR(dummy.c)
AC_CONFIG_HEADER([_config.h])

AC_PROG_CC
AC_PROG_CXX

SING_RESET_FLAGS()

AM_MAINTAINER_MODE([enable])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects]) # -Wno-extra-portability -Werror silent-rules
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

AM_SANITY_CHECK

# Add pre'prefixed config
AX_PREFIX_CONFIG_H([omConfig.h],[],[_config.h])

# SING_CHECK_SET_ARGS()

AC_SUBST(VERSION)

CPPFLAGS="-I.. -I. $CPPFLAGS"
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CPP

AC_PROG_LN_S
AC_PROG_INSTALL
AM_PROG_CC_C_O
# AM_PROG_AR
AC_C_CONST
AC_C_INLINE

LT_INIT([win32-dll])

SING_WIN_FLAGS


dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl check for programs
dnl
AC_CHECK_PROG(AR, ar, ar, :)
if test "$ac_cv_prog_AR" = no; then
  AC_MSG_ERROR(*** ar program not found)
fi

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl header file checks
dnl
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h,,
  AC_MSG_ERROR(Can not compile without limits.h))

AC_CHECK_HEADERS(unistd.h sys/mman.h fcntl.h /usr/include/malloc.h)

AC_CHECK_FUNCS(popen mmap sbrk random)

dnl llllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl Find out more about particularity of the system
dnl

# sizes
AC_CHECK_SIZEOF(long,4)
AC_CHECK_SIZEOF(void*,4)
AC_CHECK_SIZEOF(double, 8)
AC_CHECK_SIZEOF(size_t, 4)

if test "$ac_cv_sizeof_long" != "$ac_cv_sizeof_voidp"; then
  AC_MSG_ERROR(need equal sizes for long and void*)
fi
if test "$ac_cv_sizeof_voidp" != 4 && test "$ac_cv_sizeof_voidp" != 8; then
  AC_MSG_ERROR(need void* to be 4 or 8 bytes long)
fi
if test "$ac_cv_sizeof_double" != 4 && test "$ac_cv_sizeof_double" != 8; then
  AC_MSG_ERROR(need double to be 4 or 8 bytes long)
fi

dnl Set compiler, linker flags so that we can work with omalloc
BACKUP_CFLAGS=$CFLAGS
BACKUP_CXXFLAGS=$CXXFLAGS
BACKUP_LDFLAGS=$LDFLAGS

CFLAGS="$CFLAGS -I$srcdir"
CXXFLAGS="$CXXFLAGS -I$srcdir"

AC_SUBST(OM_MALLOC_HEADER)
AC_SUBST(OM_MALLOC_SOURCE)

dnl lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll
dnl wrap it up
dnl
AC_CONFIG_FILES([Makefile omalloc.pc])
AC_OUTPUT

