AC_DEFUN([LB_CHECK_MATHICGB],
[
#
 AC_ARG_WITH(mathicgb,
   [ 
    AS_HELP_STRING( 
     [--with-mathicgb=yes|no], 
     [Use the MathicGB library. Default is no.] 
    )
   ],
   [], 
   [with_mathicgb="no"] 
 )
 #    
 # MathicGB
 dnl Checking these pre-requisites and adding them to libs is necessary
 dnl for some reason, at least on Cygwin.
 AS_IF( [test "x$with_mathicgb" = xyes], 
 [
  AC_CHECK_LIB([tbb], [TBB_runtime_interface_version], [], [])
  AC_CHECK_LIB(memtailor, libmemtailorIsPresent, [],
    [AC_MSG_ERROR([Cannot find libmemtailor, which is required for MathicGB.])])
  AC_CHECK_LIB(mathic, libmathicIsPresent, [], 
    [AC_MSG_ERROR([Cannot find libmathic, which is required for MathicGB.])])
  AC_CHECK_LIB(mathicgb, libmathicgbIsPresent, [],
    [AC_MSG_ERROR([Cannot find the MathicGB library.])], [-lstdc++])
  AC_CHECK_HEADER([mathicgb.h])
  AH_TEMPLATE([HAVE_MATHICGB], [have mathicgb])
  AC_DEFINE(HAVE_MATHICGB)
 ])
 #
])
