# -*- mode: cmake; -*-
#

#
#
#
function(MP_mk_header GEN_H_FILE)
  set(_GEN_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/MP.h)
  set(hdir "${CMAKE_CURRENT_SOURCE_DIR}/h")
  set(INFILE "${hdir}/MP.h")

  message(STATUS "Add MP_mk_header ${_GEN_H_FILE}")
  message(STATUS "Sd=${CMAKE_CURRENT_SOURCE_DIR}")
  message(STATUS "Shdir=${hdir}")
  message(STATUS "Sin=${INFILE}")
  message(STATUS "cmd: perl ${CMAKE_CURRENT_SOURCE_DIR}/makeheader.pl -I${hdir} ${INFILE} ${_GEN_H_FILE}")
  add_custom_command(OUTPUT ${_GEN_H_FILE}
    COMMAND perl ${CMAKE_SOURCE_DIR}/MP/MP/makeheader.pl -I${hdir} ${INFILE} ${_GEN_H_FILE}

    DEPENDS ${INFILE}
    COMMENT "Compiling '${INFILE}'..."
    )

  set(${GEN_H_FILE}   ${_GEN_H_FILE}   PARENT_SCOPE)
endfunction()
