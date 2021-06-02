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
 AC_MSG_WARN([Please note that we set empty defaults for CFLAGS and CXXFLAGS])
 : ${CFLAGS:=""}
 : ${CXXFLAGS:=""}
])


AC_DEFUN([SING_CHECK_SET_ARGS], [
#  SING_SHOW_FLAGS([Initial state?...])dnl

 AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [build the debugging version of the libraries]),
  [ENABLE_DEBUG="$enableval"], [ENABLE_DEBUG=""])

 AC_MSG_CHECKING([debugging checks should be embedded])
 if test "x${ENABLE_DEBUG}" != xyes; then
  AC_MSG_RESULT([no])
 else
  AC_MSG_RESULT([yes])
 fi

 AC_ARG_ENABLE([optimizationflags],
  AS_HELP_STRING([--disable-optimizationflags], [build the without default optimization flags]),
  [ENABLE_OPTIMIZATION="$enableval"], [ENABLE_OPTIMIZATION="yeah"])

 if test "x${ENABLE_DEBUG}" = xyes; then
  SINGULAR_CFLAGS=""
  if test "x${ENABLE_OPTIMIZATION}" = xyeah; then
   ENABLE_OPTIMIZATION="no"
   AC_MSG_WARN([Please note that we disable implicit (default) optimization flags since you have enabled the debug flags... ])
  fi
 else
  SINGULAR_CFLAGS="-DSING_NDEBUG -DOM_NDEBUG"
  # for now let '-DSING_NDEBUG -DOM_NDEBUG' be here...
  AC_DEFINE([OM_NDEBUG],1,"Disable OM Debug")
  AC_DEFINE([SING_NDEBUG],1,"Disable Singular Debug")
 fi

 if test "x${ENABLE_OPTIMIZATION}" = xyeah; then
   ENABLE_OPTIMIZATION="yes"
 fi

 if test "x${ENABLE_OPTIMIZATION}" = xyes; then
  if test "x${ENABLE_DEBUG}" = xyes; then
   AC_MSG_WARN([Please note that you will be using our optimization flags together with debug flags... ])
  fi
 fi

 AC_MSG_CHECKING([whether optimization flags should be used])
 if test "x${ENABLE_OPTIMIZATION}" = xyes; then
  AC_MSG_RESULT([yes])
 else
  AC_MSG_RESULT([no])
 fi


 AM_CONDITIONAL(WANT_DEBUG, test x"${ENABLE_DEBUG}" = xyes)
 AM_CONDITIONAL(WANT_OPTIMIZATIONFLAGS, test x"${ENABLE_OPTIMIZATION}" = xyes)

 AC_DEFINE_UNQUOTED([SINGULAR_CFLAGS],"$SINGULAR_CFLAGS",[SINGULAR_CFLAGS])
 AC_SUBST(SINGULAR_CFLAGS)

# SING_SHOW_FLAGS([checking flags....])

 FLAGS="-pipe -fno-common"
 AC_LANG_PUSH([C])
 AX_APPEND_COMPILE_FLAGS(${FLAGS}, [CFLAGS])
 AC_LANG_POP([C])

 AC_LANG_PUSH([C++])
 AX_APPEND_COMPILE_FLAGS(${FLAGS}, [CXXFLAGS])

 # separate parts of Singular have to be real C++ :)
 FFLAGS="-fexceptions -frtti"
 AX_APPEND_COMPILE_FLAGS(${FFLAGS}, [FEXCEPTIONSFRTTI_CXXFLAGS])
 AC_SUBST(FEXCEPTIONSFRTTI_CXXFLAGS)

 AC_LANG_POP([C++])

 AX_APPEND_LINK_FLAGS(${FLAGS})

 if test "x${ENABLE_DEBUG}" = xyes; then
  DBGFLAGS="-g -ftrapv -fdiagnostics-show-option -Wall -Wextra -fno-delete-null-pointer-checks"
  #  -pedantic too strict ??? -Wvla -Wno-long-long ???
  AC_LANG_PUSH([C])
  AX_APPEND_COMPILE_FLAGS(${DBGFLAGS}, [CFLAGS])
  AC_LANG_POP([C])
  AC_LANG_PUSH([C++])
  AX_APPEND_COMPILE_FLAGS(${DBGFLAGS}, [CXXFLAGS])
  AC_LANG_POP([C++])
  AX_APPEND_LINK_FLAGS(${DBGFLAGS})
 fi

 ## for clang: -Wunneeded-internal-declaration

 if test "x${ENABLE_OPTIMIZATION}" != xno; then
  OPTFLAGS="-g0 -O3 -Wno-unused-function -Wno-trigraphs -Wno-unused-parameter -Wunknown-pragmas -Wno-unused-variable -fomit-frame-pointer -fwrapv -fvisibility=default -finline-functions -fno-exceptions -fno-threadsafe-statics -fno-enforce-eh-specs -fconserve-space -funroll-loops -fno-delete-null-pointer-checks"
  #  -O3 - crashes gcc???!!!
  # -fpermissive
  AC_LANG_PUSH([C])
  AX_APPEND_COMPILE_FLAGS(${OPTFLAGS}, [CFLAGS])
  AC_LANG_POP([C])
  AC_LANG_PUSH([C++])
  AX_APPEND_COMPILE_FLAGS(${OPTFLAGS}, [CXXFLAGS])
  AX_APPEND_COMPILE_FLAGS([-fno-rtti], [CXXFLAGS])
#   AX_APPEND_COMPILE_FLAGS([-fno-threadsafe-statics -fno-enforce-eh-specs -fconserve-space], [CXXFLAGS])
###  AX_APPEND_COMPILE_FLAGS([-fno-implicit-templates], [CXXFLAGS]) # problems due to STL
  AC_LANG_POP([C++])
  AX_APPEND_LINK_FLAGS(${OPTFLAGS})
#  AX_APPEND_LINK_FLAGS([-fno-threadsafe-statics -fno-enforce-eh-specs -fconserve-space])
###  AX_APPEND_LINK_FLAGS([-fno-implicit-templates]) # see above :(
#  AX_APPEND_LINK_FLAGS([ ])
 fi

 FLAGS2="-Qunused-arguments"
 AC_LANG_PUSH([C])
 AX_APPEND_COMPILE_FLAGS(${FLAGS2}, [CFLAGS])
 AC_LANG_POP([C])

 AC_LANG_PUSH([C++])
 AX_APPEND_COMPILE_FLAGS(${FLAGS2}, [CXXFLAGS])
 AC_LANG_POP([C++])

 AX_APPEND_LINK_FLAGS(${FLAGS2})

# SING_SHOW_FLAGS([before PROG_C_CC])


AC_PROG_CC
AC_PROG_CXX
])
