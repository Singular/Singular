#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([polys.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
#AC_PROG_RANLIB
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile])

case $DL_KERNEL in
 static)        DL_KERNEL_OBJECT='p_Procs_Static.$(OBJEXT)'
 		DL_KERNEL_OBJECT_DEBUG='p_Procs_Static.$(OBJEXT)g' ;;
 dynamic)       DL_KERNEL_OBJECT='p_Procs_Dynamic.$(OBJEXT)'
 	        DL_KERNEL_OBJECT_DEBUG='p_Procs_Dynamic.$(OBJEXT)g' ;;
esac

AC_SUBST([DL_KERNEL_OBJECT])
AC_SUBST([DL_KERNEL_OBJECT_DEBUG])

AC_OUTPUT
