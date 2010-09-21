# -*- mode: cmake; -*-
#
#
#

#
#
#
macro(omalloc_create_omalloc_h TARGET GEN_H_FILE CMD INFILE DEPEND)
  #set(_GEN_TARGET  "gen-header-omallocH")
  set(GEN_H_FILE  ${CMAKE_CURRENT_BINARY_DIR}/omalloc.h)
  message(STATUS "Creating '${GEN_H_FILE}'")
  add_custom_command(
    OUTPUT  ${GEN_H_FILE}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/makeheader ${INFILE} ${GEN_H_FILE}
    DEPENDS ${INFILE} ${DEPEND}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Creating '${GEN_H_FILE}'..."
    )
  add_custom_target(${TARGET} DEPENDS ${GEN_H_FILE})
  #set(${GEN_TARGET} ${_GEN_TARGET} PARENT_SCOPE)
  #set(${GEN_H_FILE} ${_GEN_H_FILE} PARENT_SCOPE)

endmacro()
