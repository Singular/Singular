# -*- mode: cmake; -*-
#
#
#

function(ip_create_version GET_TARGET GET_FILE)
  message(STATUS "generate si_gcc_v.h")

  set(_GEN_TARGET "ip-inc")
  set(_GEN_FILE "${CMAKE_CURRENT_BINARY_DIR}/si_gcc_v.h")

  if(${CMAKE_CXX_COMPILER_MAJOR} EQUAL 2)
    file(WRITE ${_GEN_FILE} "#define SI_GCC2\n")
  endif(${CMAKE_CXX_COMPILER_MAJOR} EQUAL 2)
  if(${CMAKE_CXX_COMPILER_MAJOR} EQUAL 3)
    file(WRITE ${_GEN_FILE} "#define SI_GCC3\n")
  endif(${CMAKE_CXX_COMPILER_MAJOR} EQUAL 3)
  if(${CMAKE_CXX_COMPILER_MAJOR} EQUAL 4)
    file(WRITE ${_GEN_FILE} "#define SI_GCC4\n")
  endif(${CMAKE_CXX_COMPILER_MAJOR} EQUAL 4)

  set(${GEN_TARGET} ${_GEN_TARGET} PARENT_SCOPE)
  set(${GEN_FILE} ${_GEN_FILE} PARENT_SCOPE)
endfunction()

include(CheckCSourceRuns)

function(ip_create_version2 GEN_TARGET GEN_FILE)
  message(STATUS "generate si_gcc_v.h")

  set(_GEN_TARGET "ip-inc")
  set(_GEN_FILE "${CMAKE_CURRENT_BINARY_DIR}/si_gcc_v.h")
  set(TESTFILE "${CMAKE_CURRENT_BINARY_DIR}/tmp.c")
  set(_TEST_SOURCE "#include <stdio.h>
int main() {
FILE *f=fopen(\"${_GEN_FILE}\",\"w\");
#ifdef __GNUC__
char *p=__VERSION__;
if (p[0]=='2') fprintf(f,\"#define SI_GCC2\\n\");
else if ((p[0]=='3')||(p[0]=='4')) fprintf(f,\"#define SI_GCC3\\n\");
#endif
fclose(f);
return 0;
 }
")
  CHECK_C_SOURCE_RUNS(${_TEST_SOURCE} RUN_RESULT_VAR)
  message(STATUS "compile : '${_GEN_FILE}'")
  message(STATUS "run     : '${RUN_RESULT_VAR}'")
  message(STATUS "rc     : '${_TEST_SOURCE}'")

  add_custom_target(ip-inc DEPENDS ${_GEN_FILE} )
  set(${GEN_TARGET} ${_GEN_TARGET} PARENT_SCOPE)
  set(${GEN_FILE} ${_GEN_FILE} PARENT_SCOPE)
endfunction()