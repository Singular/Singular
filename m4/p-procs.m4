# pprocs.m4
# Copyright 2011 Bradford Hovinen <hovinen@gmail.com>
#
# Macro to check user-preferences and system-settings and enable or
# disable static or dynamic modules for polynomial-operations

AC_DEFUN([SING_SYSTEM_SUPPORTS_DYNAMIC_MODULES],
[
AC_CANONICAL_HOST
AC_MSG_CHECKING(whether system supports dynamic modules)
AS_CASE([$host],
  [*linux*], [SUPPORTS_DYNAMIC_MODULES=yes],
  [*freebsd*], [SUPPORTS_DYNAMIC_MODULES=yes],
  [*-sun-solaris2*], [SUPPORTS_DYNAMIC_MODULES=yes],
  [*-apple-darwin*], [SUPPORTS_DYNAMIC_MODULES=yes],
  [SUPPORTS_DYNAMIC_MODULES=no]
)
AC_MSG_RESULT($SUPPORTS_DYNAMIC_MODULES)
])

AC_DEFUN([SING_CHECK_P_PROCS],
[
AC_ARG_ENABLE(p-procs-static,
[  --enable-p-procs-static Enable statically compiled p_Procs-modules
],
[if test $enableval = yes; then
     ENABLE_P_PROCS_STATIC="yes"
     ENABLE_P_PROCS_DYNAMIC="no"
 else
     ENABLE_P_PROCS_STATIC="no"
 fi
],[NO_P_PROCS_STATIC_GIVEN=yes])

AC_ARG_ENABLE(p-procs-dynamic,
[  --enable-p-procs-dynamic Enable dynamically compiled p_Procs-modules
],
[if test $enableval = yes; then
     ENABLE_P_PROCS_DYNAMIC="yes"
     ENABLE_P_PROCS_STATIC="no"
 else
     ENABLE_P_PROCS_DYNAMIC="no"
 fi
],[NO_P_PROCS_DYNAMIC_GIVEN=yes])

if test x$ENABLE_P_PROCS_DYNAMIC = xyes; then
  SING_SYSTEM_SUPPORTS_DYNAMIC_MODULES
  if test $SUPPORTS_DYNAMIC_MODULES = no; then
    AC_MSG_ERROR([--enable-pprocs-dynamic requested but your system appears not to support dynamic modules properly])
  fi
elif test x$NO_P_PROCS_DYNAMIC_GIVEN = xyes -a x$NO_P_PROCS_STATIC_GIVEN = xyes; then
  SING_SYSTEM_SUPPORTS_DYNAMIC_MODULES
  if test $SUPPORTS_DYNAMIC_MODULES = yes; then
    AC_MSG_NOTICE([Enabling dynamic modules and disabling static modules])
    ENABLE_P_PROCS_DYNAMIC="yes"
    ENABLE_P_PROCS_STATIC="no"
  elif test $SUPPORTS_DYNAMIC_MODULES = no; then
    AC_MSG_NOTICE([Enabling static modules and disabling dynamic modules])
    ENABLE_P_PROCS_DYNAMIC="no"
    ENABLE_P_PROCS_STATIC="yes"
  else
    AC_MSG_ERROR([Unknown whether system supports dynamic modules or not. This should not have happened.])
  fi
fi

if test x$ENABLE_P_PROCS_DYNAMIC = xyes; then
  AC_DEFINE(HAVE_DL,1,enable dynamic modules)
fi

AM_CONDITIONAL([ENABLE_P_PROCS_DYNAMIC],[test x$ENABLE_P_PROCS_DYNAMIC = xyes])
AM_CONDITIONAL([ENABLE_P_PROCS_STATIC],[test x$ENABLE_P_PROCS_STATIC = xyes])

])
