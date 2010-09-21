# -*- mode: cmake; -*-
#
#
#
macro(check_header HEADER)
  STRING(TOUPPER ${HEADER} ENABLE_FLAG)
  STRING(REGEX REPLACE "/" "_" ENABLE_FLAG ${ENABLE_FLAG})
  STRING(REGEX REPLACE ".H$" "_H" ENABLE_FLAG ${ENABLE_FLAG})

  FIND_PATH(HAVE_${ENABLE_FLAG}
     NAMES ${HEADER}
     PATHS
     ${CMAKE_ADDITIONAL_PATH}
#     ${CMAKE_INCLUDE_PATH}
     /usr/local/include
     /usr/include
  )
  if( "${HAVE_${ENABLE_FLAG}}" STREQUAL "HAVE_${ENABLE_FLAG}-NOTFOUND" )
    message(STATUS "Checking for ${HEADER} 'HAVE_${ENABLE_FLAG}' ... not found")
    check_cpp_header(${HEADER})
  else()
    message(STATUS "Checking for ${HEADER} 'HAVE_${ENABLE_FLAG}' ... found")
  endif()
endmacro()

macro(check_cpp_header HEADER)
  STRING(TOUPPER ${HEADER} ENABLE_FLAG)
  STRING(REGEX REPLACE "/" "_" ENABLE_FLAG ${ENABLE_FLAG})
  STRING(REGEX REPLACE ".H$" "_H" ENABLE_FLAG ${ENABLE_FLAG})
  
  set(_srcfile ${CMAKE_BINARY_DIR}/temp.cpp)
  set(srccode "#include <${HEADER}>\nint main() { return 0\;}")
  file(WRITE ${_srcfile} ${srccode})
  #message(STATUS "source='${srccode}'")
  set(lang CXX)
  #  try_run(RUN_RESULT_VAR COMPILE_RESULT_VAR
  #          ${CMAKE_BINARY_DIR} ${_srcfile})

  try_compile(RESULT_VAR ${CMAKE_BINARY_DIR} ${_srcfile}
    OUTPUT_VARIABLE _output
    )
  #              [CMAKE_FLAGS <Flags>]
  #              [COMPILE_DEFINITIONS <flags> ...]
  #              [OUTPUT_VARIABLE var]
  #              [COPY_FILE <filename> )
  #try_run(RUN_RESULT_VAR COMPILE_RESULT_VAR
  #          bindir srcfile [CMAKE_FLAGS <Flags>]
  #          [COMPILE_DEFINITIONS <flags>]
  #          [COMPILE_OUTPUT_VARIABLE comp]
  #          [RUN_OUTPUT_VARIABLE run]
  #          [OUTPUT_VARIABLE var]
  #          [ARGS <arg1> <arg2>...])

  #  message(STATUS "Compiling ${HEADER} with result '${_output}'")
  #  message(STATUS "Compiling ${HEADER} with result '${RESULT_VAR}'")
  if(RESULT_VAR)
    set(HAVE_${ENABLE_FLAG} "/usr" CACHE PATH "" FORCE)
  endif(RESULT_VAR)
  if( "${HAVE_${ENABLE_FLAG}}" STREQUAL "HAVE_${ENABLE_FLAG}-NOTFOUND" )
    message(STATUS "Checking for ${HEADER} 'HAVE_${ENABLE_FLAG}' ... not found")
  else()
    message(STATUS "Checking for ${HEADER} 'HAVE_${ENABLE_FLAG}' ... found")
  endif()
#  message(STATUS "Compiling ${HEADER} with result '${COMPILE_RESULT_VAR}'")
#  message(STATUS "Compiling ${HEADER} with result '${RUN_RESULT_VAR}'")
endmacro()
#
#
#
function(check_headers HEADERS)
  message(STATUS "Headers '${ARGV}'")
  foreach(loop_var ${ARGV})
    string(STRIP ${loop_var} HEADER)
    check_header(${HEADER})
  endforeach(loop_var)
endfunction()

#
#
#
function(check_funcs FUNCS)
  foreach(loop_var ${ARGV})
    string(STRIP ${loop_var} FUNC)
    string(TOUPPER ${loop_var} VAR)
    check_function_exists(${FUNC} HAVE_${VAR})
  endforeach(loop_var)
endfunction()

#
#
#
function(check_library NAME )
#  message(STATUS "libray '${NAME}'")
#  ac_have_lib_NAME

  message(STATUS "Checking library ${NAME}...")
  string(TOUPPER ${NAME} VAR)
  SET(CMAKE_FIND_LIBRARY_SUFFIXES .a )# ${CMAKE_FIND_LIBRARY_SUFFIXES})
  FIND_LIBRARY(ac_have_lib_${VAR}
    NAMES ${NAME}
    PATHS ${CMAKE_LIBRARY_PATH} /usr/lib /usr/local/lib
    PATH_SUFFIXES lib
  )
  message(STATUS "Checking library ${NAME}... ${ac_have_lib_${VAR}}")
  if(ac_have_lib_${VAR})
    set(HAVE_LIB${VAR} 1 )
  else(ac_have_lib_${VAR})
    set(HAVE_LIB${VAR} 0 )
  endif(ac_have_lib_${VAR})

  if(NOT HAVE_LIB${VAR})
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .so )# ${CMAKE_FIND_LIBRARY_SUFFIXES})
    FIND_LIBRARY(ac_have_lib_${VAR}_DL
      NAMES ${NAME}
      PATHS ${CMAKE_LIBRARY_PATH} /usr/lib /usr/local/lib
      PATH_SUFFIXES lib
      )
    message(STATUS "Checking library ${NAME}... ${ac_have_lib_${VAR}_DL}")
  endif(NOT HAVE_LIB${VAR})

  if(ac_have_lib_${VAR}_DL)
    set(HAVE_LIB${VAR}_DL 1 )
  else(ac_have_lib_${VAR}_DL)
    set(HAVE_LIB${VAR}_DL 0 )
  endif(ac_have_lib_${VAR}_DL)

  set(HAVE_LIB${VAR} ${HAVE_LIB${VAR}} PARENT_SCOPE)
  set(HAVE_LIB${VAR}_DL ${HAVE_LIB${VAR}_DL} PARENT_SCOPE)

message(STATUS "1 HAVE_LIB${VAR} =" ${HAVE_LIB${VAR}})
message(STATUS "1 HAVE_LIB${VAR}_DL =" ${HAVE_LIB${VAR}_DL})
endfunction(check_library NAME)
