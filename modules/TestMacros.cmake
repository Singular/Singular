# -*- mode: cmake; -*-
#
#
#

function(singular_create_test GEN_TARGET GEN_FILE CMD INFILE) 

  message("create testsuite...")
#  set(${GEN_FILE} ${INFILE} PARENT_SCOPE)
  string(REGEX REPLACE "/" "-" _NAME ${INFILE})
  set(_GEN_TARGET "${_NAME}-gen")
  set(_GEN_FILE   "${CMAKE_CURRENT_BINARY_DIR}/${_NAME}")
  set(_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/regress.cmd")
  set(_TESTFILE "${CMAKE_CURRENT_SOURCE_DIR}/${INFILE}")
#  add_custom_command(OUTPUT ${_GEN_FILE}
#    COMMAND ${PERL_CMD} ${_SCRIPT} -r -s ${CMD} ${_TESTFILE}
#    COMMAND touch ${_GEN_FILE}
#
#    DEPENDS ${INFILE} ${CMD} ${_SCRIPT}
#    COMMENT "Running testsuite '${INFILE}'..."
#    )
  add_custom_target(${_GEN_TARGET} ALL
    COMMAND ${PERL_CMD} ${_SCRIPT} -r -s ${CMD} ${_TESTFILE}
    DEPENDS ${INFILE} ${CMD} ${_SCRIPT}
    COMMENT "Running testsuite '${INFILE}'..."
    )

#  add_custom_target(${_GEN_TARGET} DEPENDS ${_GEN_FILE} )

#add_custom_target(Name [ALL] [command1 [args1...]]
#                    [COMMAND command2 [args2...] ...]
#                    [DEPENDS depend depend depend ... ]
#                    [WORKING_DIRECTORY dir]
#                    [COMMENT comment] [VERBATIM]
#                    [SOURCES src1 [src2...]])
  set(${GEN_FILE}   ${_GEN_FILE}   PARENT_SCOPE)
  set(${GEN_TARGET} ${_GEN_TARGET} PARENT_SCOPE)

endfunction()
