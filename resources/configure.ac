AC_INIT([libresources], [3.1.4.sw])

AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([omFindExec.h])
AC_CONFIG_HEADER([config.h])

AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects]) # -Wno-extra-portability -Werror silent-rules
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

dnl Check if build env is sane
AM_SANITY_CHECK

SING_RESET_FLAGS()
SING_CHECK_SET_ARGS()

AM_PROG_CC_C_O
# AM_PROG_AR

AC_PROG_LN_S
AC_PROG_INSTALL

AC_FUNC_MALLOC
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h pwd.h sys/param.h sys/stat.h sys/types.h])
AC_CHECK_FUNCS(readlink getcwd getwd getpwnam setenv putenv)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Turn off shared libraries during beta-testing, since they
# make the build process take too long.
LT_INIT
# ([shared])
# LT_INIT(dlopen disable-static) # doesn't work on PowerPC!

# SING_CHECK_PIPE

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
