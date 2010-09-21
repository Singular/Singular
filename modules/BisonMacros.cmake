# -*- mode: cmake; -*-
#
#
#

function(add_grammar  GEN_TARGET GEN_CPP_FILE GEN_H_FILE INFILE)
  string(REGEX REPLACE "^.*/" "" FILE ${INFILE})
  string(REPLACE ".y" "" NAME ${FILE})
  set(_GEN_TARGET      "bison-${NAME}")
  set(_GEN_CPP_FILE    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp)
  set(_GEN_H_FILE      ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.h)
  set(_GEN_GRAPH_FILE  ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.graph)
  set(_GEN_XML_FILE    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.xml)
  set(_GEN_REPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.report)
  set(_SOURCE_CPP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.cc)
  set(_SOURCE_H_FILE   ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.h)
  #set(_OPT "-y -d -t")
  # -y, --yacc : emulate POSIX Yacc
  # -t, --debug: instrument the parser for debugging
  # -d : likewise but cannot specify FILE (for POSIX Yacc)
  # -r, --report
  # --report-file=FILE
  # --graph=FILE
  # --xml=FILE
  set(_OPT -y -d -t)
  #set(_OPT ${_OPT} --warning=all)
  #set(_OPT ${_OPT} --xml=${_GEN_XML_FILE})
  #set(_OPT ${_OPT} --graph=${_GEN_GRAPH_FILE})
  #set(_OPT ${_OPT} --report=all --report-file=${_GEN_REPORT_FILE})

  set(BISON bison)

  message("add_yacc ${_GEN_CPP_FILE} ${INFILE}...")
  message("add_yacc ${_GEN_H_FILE} ${INFILE}...")
  message("add_yacc ${NAME}...")

#    COMMAND chmod a+x ${CMAKE_CURRENT_SOURCE_DIR}/testgh
#    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testgh
  add_custom_command(OUTPUT ${_GEN_CPP_FILE}
    COMMAND ${BISON} ${_OPT} -o ${_GEN_CPP_FILE} ${INFILE}
    COMMAND mv ${NAME}.hpp ${_GEN_H_FILE}
     
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_SOURCE_CPP_FILE} ${_GEN_CPP_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_SOURCE_H_FILE} ${_GEN_H_FILE}

    DEPENDS ${INFILE}
    COMMENT "Compiling '${INFILE}'..."
    COMMENT "run ${BISON} ${_OPT} -o ${_GEN_CPP_FILE} ${INFILE}"
    )

  set(${GEN_TARGET} ${_GEN_TARGET} PARENT_SCOPE)
  set(${GEN_CPP_FILE} ${_GEN_CPP_FILE} PARENT_SCOPE)
  set(${GEN_H_FILE}   ${_GEN_H_FILE}   PARENT_SCOPE)
endfunction()

