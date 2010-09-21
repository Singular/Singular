# -*- mode: cmake; -*-
#
#
#

function(mk_prcopy GEN_FILE INFILE) 

  message("mk_prcopy...")
#  set(${GEN_FILE} ${INFILE} PARENT_SCOPE)
  set(${GEN_FILE} "${CMAKE_CURRENT_BINARY_DIR}/prCopy.inc")
  add_custom_command(OUTPUT ${${GEN_FILE}}
    COMMAND perl ${INFILE} > ${${GEN_FILE}}

    DEPENDS ${INFILE}
    COMMENT "Compiling '${INFILE}'..."
    )
  message(STATUS "argv: ${ARGV}")
  message(STATUS "argn: ${ARGC}")
  message(STATUS "argn: '${ARGN}'")


  set(${GEN_FILE} ${${GEN_FILE}} PARENT_SCOPE)
endfunction()

#
#
#
function(mk_procs GEN_FILE PROC INFILE )
  message("mk_procs ${INFILE}...")

  #set(srcs p_Procs_${INFILE}.cc p_Procs_Generate.cc)
  #set(proc p_Procs_${INFILE})
  #add_executable(${proc} ${srcs})

  set(_GEN_FILE "${CMAKE_CURRENT_BINARY_DIR}/p_Procs_${INFILE}.inc")

  add_custom_command(OUTPUT ${_GEN_FILE}
        COMMAND ${PROC} > ${_GEN_FILE}

    DEPENDS ${PROC}
    COMMENT "Creating '${_GEN_FILE}'..."
    )
#        -rm -f p_Procs_$* p_Procs_$*.exe
#    COMMAND         $(CXX) -g -Dp_Procs_$* p_Procs_Generate.cc -o p_Procs_$*
  set(${GEN_FILE} ${_GEN_FILE} PARENT_SCOPE)
  message(STATUS "  mk_procs done")

endfunction()

