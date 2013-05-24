AC_DEFUN([SING_SHOW_FLAGS], [

echo "/* --------------- $1 --------------- */";

AC_MSG_CHECKING([  CFLAGS?..])
AC_MSG_RESULT(${CFLAGS:-unset})
AC_MSG_CHECKING([CXXFLAGS?..])
AC_MSG_RESULT(${CXXFLAGS:-unset})
AC_MSG_CHECKING([CPPFLAGS?..])
AC_MSG_RESULT(${CPPFLAGS:-unset})
AC_MSG_CHECKING([    DEFS?..])
AC_MSG_RESULT(${DEFS:-unset})
AC_MSG_CHECKING([ LDFLAGS?..])
AC_MSG_RESULT(${LDFLAGS:-unset})
AC_MSG_CHECKING([    LIBS?..])
AC_MSG_RESULT(${LIBS:-unset})
AC_MSG_CHECKING([     GCC?..])
AC_MSG_RESULT(${GCC:-unset})
AC_MSG_CHECKING([      CC?..])
AC_MSG_RESULT(${CC:-unset})
AC_MSG_CHECKING([     GXX?..])
AC_MSG_RESULT(${GXX:-unset})
AC_MSG_CHECKING([     CXX?..])
AC_MSG_RESULT(${CXX:-unset})

# echo "/* =============== $1 =============== */";
])

AC_DEFUN([SING_RESET_FLAGS], [
 AC_MSG_WARN([Please note that we set empty defaults for \`CFLAGS' and \`CXXFLAGS' (instead of \`-g -O')])
 : ${CFLAGS:=""}
 : ${CXXFLAGS:=""}
])


AC_DEFUN([SING_CHECK_SET_ARGS], [
#  SING_SHOW_FLAGS([Initial state?...])dnl

 ENABLE_DEBUG="no"
 ENABLE_OPTIMIZATION="yes"

 AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [build the debugging version of the libraries]),
  [if test "x$enableval" = "xyes"; then
   ENABLE_DEBUG="yes"
  fi])

 AC_ARG_ENABLE([optimizationflags],
  AS_HELP_STRING([--disable-optimizationflags], [build the without default optimization flags]),
  [if test "x$enableval" = "xno"; then
   ENABLE_OPTIMIZATION="no"
  fi])
  
 AC_MSG_CHECKING([debugging checks should be embedded])
 if test "x${ENABLE_DEBUG}" != xyes; then
  AC_MSG_RESULT([no])
 else
  AC_MSG_RESULT([yes])
 fi
 
 AC_MSG_CHECKING([whether optimization flags should be used])
 if test "x${ENABLE_OPTIMIZATION}" == xyes; then
  AC_MSG_RESULT([yes])
 else
  AC_MSG_RESULT([no])
 fi
 
 AM_CONDITIONAL(WANT_DEBUG, test x"${ENABLE_DEBUG}" == xyes)
 AM_CONDITIONAL(WANT_OPTIMIZATIONFLAGS, test x"${ENABLE_OPTIMIZATION}" != xno)

 if test "x${ENABLE_DEBUG}" == xyes; then
  if test "x${ENABLE_OPTIMIZATION}" == xyes; then
   AC_MSG_ERROR([Please note that you have enabled the debug and optimization flags, which may not work well together (try \`--disable-optimizationflags')!])
  fi
 else
  AC_DEFINE([OM_NDEBUG],1,"Disable OM Debug")
  AC_DEFINE([NDEBUG],1,"Disable Debug")
 fi

# SING_SHOW_FLAGS([checking flags....])

 FLAGS="-pipe"
 AC_LANG_PUSH([C])
 AX_APPEND_COMPILE_FLAGS(${FLAGS}, [CFLAGS])
 AC_LANG_POP([C])
 
 AC_LANG_PUSH([C++])
 AX_APPEND_COMPILE_FLAGS(${FLAGS}, [CXXFLAGS])
 AC_LANG_POP([C++])

 AX_APPEND_LINK_FLAGS(${FLAGS}) 
 
 if test "x${ENABLE_DEBUG}" == xyes; then
  DBGFLAGS="-g -O0 -ftrapv -fdiagnostics-show-option -pedantic -Wall -Wextra"
  AC_LANG_PUSH([C])
  AX_APPEND_COMPILE_FLAGS(${DBGFLAGS}, [CFLAGS])
  AC_LANG_POP([C])
  AC_LANG_PUSH([C++])
  AX_APPEND_COMPILE_FLAGS(${DBGFLAGS}, [CXXFLAGS])
  AC_LANG_POP([C++])
  AX_APPEND_LINK_FLAGS(${DBGFLAGS})
 fi
 
 if test "x${ENABLE_OPTIMIZATION}" != xno; then
  OPTFLAGS="-O2 -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -fomit-frame-pointer -fwrapv"
  #  -O3 - crashes gcc???!!!  
  AC_LANG_PUSH([C])
  AX_APPEND_COMPILE_FLAGS(${OPTFLAGS}, [CFLAGS])
  AC_LANG_POP([C])
  AC_LANG_PUSH([C++])
  AX_APPEND_COMPILE_FLAGS(${OPTFLAGS}, [CXXFLAGS])
  AX_APPEND_COMPILE_FLAGS([-fno-exceptions -fno-rtti -fpermissive], [CXXFLAGS])
  # AX_APPEND_COMPILE_FLAGS([-fno-implicit-templates], [CXXFLAGS]) # problems due to STL
  AC_LANG_POP([C++])
  AX_APPEND_LINK_FLAGS(${OPTFLAGS}) 
  AX_APPEND_LINK_FLAGS([-fno-exceptions -fno-rtti -fpermissive])
#  AX_APPEND_LINK_FLAGS([-fno-implicit-templates]) # see above :(
#  AX_APPEND_LINK_FLAGS([-rdynamic]) 
 fi

# SING_SHOW_FLAGS([before PROG_C_CC])

])
