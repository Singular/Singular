# -*- mode: cmake; -*-
#
#
#
function(factory_header GEN_H_FILE TEMPLATE_FILE)
  string(REGEX REPLACE ".template" "" TEMPLATE_BASE ${TEMPLATE_FILE})
  set(_GEN_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TEMPLATE_BASE}.h)
  set(MAKEHEADER_CMD "${CMAKE_CURRENT_SOURCE_DIR}/bin/makeheader")

  set(_OPT "-I${CMAKE_CURRENT_SOURCE_DIR}")
  set(_OPT "${_OPT} -I${CMAKE_CURRENT_BINARY_DIR}")
  add_custom_command(OUTPUT ${_GEN_H_FILE}
    COMMAND ${MAKEHEADER_CMD} ${_OPT} ${CMAKE_CURRENT_SOURCE_DIR}/${TEMPLATE_FILE} ${_GEN_H_FILE}
     
    DEPENDS ${TEMPLATE_FILE}
    COMMENT "Compiling '${TEMPLATE_FILE}'..."
    )

  set(${GEN_H_FILE} ${_GEN_H_FILE} PARENT_SCOPE)
endfunction()

function(factory_gen_gmp_header GEN_TARGET GEN_H_FILE)
  set(_GEN_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/cf_gmp.h)
  #try_compile(RESULT_VAR bindir srcdir
  #            projectName <targetname> [CMAKE_FLAGS <Flags>]
  #            [OUTPUT_VARIABLE var])
  #try_compile(RESULT_VAR ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}
  #  projectName cf_gmp 
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -M ${CMAKE_CURRENT_SOURCE_DIR}/gen_cf_gmp.cc
    RESULT_VARIABLE RESULT_VAR
    OUTPUT_VARIABLE OUTPUT_VAR)
  string(REGEX REPLACE "gen_cf_gmp.o:" "" OUTPUT_VAR ${OUTPUT_VAR})
  string(REGEX REPLACE "[\\\n]" " " OUTPUT_VAR ${OUTPUT_VAR})
  string(REGEX REPLACE "[ ]+" ";" _OUTPUT_VAR ${OUTPUT_VAR})
 
  set(_gmp_headers "")

  foreach(_loop_var ${_OUTPUT_VAR})
    if( ${_loop_var} MATCHES "gmp.*h")
      message(STATUS "==${_loop_var}=")
      set(_gmp_headers ${_gmp_headers} ${_loop_var})
      #list(APPEND _gmp_headers ${_loop_var})
    endif( ${_loop_var} MATCHES "gmp.*h")
  endforeach(_loop_var)
  message(STATUS "'${_gmp_headers}'")

  add_custom_command(OUTPUT ${_GEN_H_FILE}
    COMMAND cat ${_gmp_headers} | grep -v __GMP_DECLSPEC_XX > ${_GEN_H_FILE}
    
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Create cf_gmp.h"
    )
  message(STATUS "====================================")
  message(STATUS "'${RESULT_VAR}'")
  #message(STATUS "'${_OUTPUT_VAR}'")
  message(STATUS "====================================")

  set(${GEN_TARGET} "generate_factory" PARENT_SCOPE)
  add_custom_target(generate_factory DEPENDS ${_GEN_H_FILE} )
  set(${GEN_H_FILE} ${_GEN_H_FILE} PARENT_SCOPE)
endfunction()
