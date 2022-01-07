AC_INIT([singular_resources], [4.3.0])

AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_SRCDIR([omFindExec.h])
AC_CONFIG_HEADER([_config.h])

AM_MAINTAINER_MODE([enable])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects]) # -Wno-extra-portability -Werror silent-rules
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

dnl Check if build env is sane
AM_SANITY_CHECK

# Add pre'prefixed config
AX_PREFIX_CONFIG_H([singular_resourcesconfig.h],[],[_config.h])

AM_PROG_AR

SING_RESET_FLAGS()
SING_CHECK_SET_ARGS()

AM_PROG_CC_C_O

AC_PROG_LN_S
AC_PROG_INSTALL

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
LT_INIT([win32-dll])
# ([shared])
# LT_INIT([win32-dll])(dlopen disable-static) # doesn't work on PowerPC!

# SING_CHECK_PIPE

SING_WIN_FLAGS

##### SEE http://www.gnu.org/prep/standards/html_node/Directory-Variables.html

AX_RECURSIVE_EVAL([[$]prefix], [config_prefix])
AX_NORMALIZE_PATH([config_prefix],['/'])
AC_DEFINE_UNQUOTED([PREFIX],"$config_prefix",[prefix])

AX_RECURSIVE_EVAL([[$]exec_prefix], [config_exec_prefix])
AX_NORMALIZE_PATH([config_exec_prefix],['/'])
AC_DEFINE_UNQUOTED([EXEC_PREFIX],"$config_exec_prefix",[exec_prefix])

AX_RECURSIVE_EVAL([[$]libexecdir], [config_libexecdir])
AX_NORMALIZE_PATH([config_libexecdir],['/'])
AC_DEFINE_UNQUOTED([LIBEXEC_DIR],"$config_libexecdir",[libexecdir])

AX_RECURSIVE_EVAL([[$]libdir], [config_libdir])
AX_NORMALIZE_PATH([config_libdir],['/'])
AC_DEFINE_UNQUOTED([LIB_DIR],"$config_libdir",[libdir])

AX_RECURSIVE_EVAL([[$]bindir], [config_bindir])
AX_NORMALIZE_PATH([config_bindir],['/'])
AC_DEFINE_UNQUOTED([BIN_DIR],"$config_bindir",[bindir])

AX_RECURSIVE_EVAL([[$]datadir], [config_datadir])
AX_NORMALIZE_PATH([config_datadir],['/'])
AC_DEFINE_UNQUOTED([DATA_DIR],"$config_datadir",[datadir])

AC_CONFIG_FILES([singular_resources.pc Makefile])
AC_OUTPUT
