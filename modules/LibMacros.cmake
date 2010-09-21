# -*- mode: cmake; -*-
#
# This file contains a convenience macro for the use with the SMC state machine compiler
#

function(unpack_library GEN_FILELIST GEN_TARGET SUBDIR NAME)
  set(_GEN_FILES "NIL")
  set(_GEN_TARGET "unpack-${NAME}")
  set(AR ar)
  get_target_property(loc ${NAME} LOCATION)
  message(STATUS "Merge-Unpack ${NAME} target='${_GEN_TARGET}' - '${_GEN_FILES}'")
  #add_custom_command(
  #  COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${SUBDIR}
  #  )
  execute_process(
    COMMAND mkdir ${CMAKE_CURRENT_BINARY_DIR}/${SUBDIR}
    COMMENT "Merge:Unpack:mkdir ${CMAKE_CURRENT_BINARY_DIR}/${SUBDIR}"
    )
  add_custom_target(${_GEN_TARGET}
    COMMAND ${AR} x ${loc}
    COMMAND ls -l 
    DEPENDS ${NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${SUBDIR}
    COMMENT "Merge:Unpack library ${NAME}")

  set(${GEN_TARGET} ${_GEN_TARGET} PARENT_SCOPE)
  set(${GEN_FILELIST} ${_GEN_FILES} PARENT_SCOPE)
endfunction()

function(merge_libraries GEN_TARGET GEN_LIB_A LIB_NAME LIBFILES)

  set(_GEN_TARGET "${LIB_NAME}-add")
  set(_GEN_LIB_A "${CMAKE_CURRENT_BINARY_DIR}/lib${LIB_NAME}.a" )

  message(STATUS "  Merging libraries into ${LIB_NAME}.a")
  message(STATUS "  Have ARGC=${ARGC} arguments")
  message(STATUS "  Have ARGN=${ARGN} arguments")
  message(STATUS "CBD: '${CMAKE_CURRENT_BINARY_DIR}'")

  add_custom_target(${_GEN_TARGET})
  set(i 0)
  foreach(loop_var ${ARGV})
    math(EXPR i ${i}+1)
    if( ${i} GREATER 3)
      get_target_property(loc ${loop_var} LOCATION)
      message(STATUS "    Argument ${i}: '${loop_var}' '${loc}'.")
      unpack_library(FILE_${loop_var} _GEN_TARGET_${loop_var} ${loop_var} ${loop_var})
      file(GLOB nlin_srsc ${loop_var}/*.o)
      message(STATUS "    -> filevar:FILE_${loop_var}='${FILE_${loop_var}}' target='${_GEN_TARGET_${loop_var}}'")
      add_dependencies(${_GEN_TARGET} ${_GEN_TARGET_${loop_var}})
      #set(arg "ARG${loop_var}")
    #message(STATUS "    Argument ${loop_var}: '${arg2}'.")
    #string(STRIP ${loop_var} FUNC)
    #string(TOUPPER ${loop_var} VAR)
    #check_function_exists(${FUNC} HAVE_${VAR})
    endif()
  endforeach(loop_var)

  add_custom_command(TARGET ${_GEN_TARGET}
    COMMAND echo "FileS: ${FILE_${loop_var}}'"
    COMMENT "Merge: rebuild library"
    )
  set(${GEN_TARGET} ${_GEN_TARGET} PARENT_SCOPE)
  set(${GEN_LIB_A} ${_GEN_LIB_A} PARENT_SCOPE)
endfunction()
