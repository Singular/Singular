# -*- mode: cmake; -*-
#
#
#

function(add_lex GEN_CPP_FILE INFILE PREFIX)
  string(REGEX REPLACE "^.*/" "" FILE ${INFILE})
  string(REPLACE ".l" "" NAME ${FILE})

  set(_GEN_CPP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp)
  set(_SOURCE_CPP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.cc)

  message("add_lex ${${GEN_CPP_FILE}} ${INFILE}...")

  if(NOT ${PREFIX} STREQUAL "")
    set(OPT "-P${PREFIX}")
    set(_orig ${CMAKE_CURRENT_SOURCE_DIR}/libparse.cc)
  else(NOT ${PREFIX} STREQUAL "")
    set(OPT "-s")
    set(_orig ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cc)
  endif(NOT ${PREFIX} STREQUAL "")

  add_custom_command(OUTPUT ${_GEN_CPP_FILE}
    COMMAND ${FLEX} ${OPT} -o ${_GEN_CPP_FILE}.tmp  ${INFILE}
    #    COMMAND cp ${_SOURCE_CPP_FILE}  ${_GEN_CPP_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_GEN_CPP_FILE}.tmp ${_GEN_CPP_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_SOURCE_CPP_FILE} ${_GEN_CPP_FILE}
    COMMAND ls -l ${_GEN_CPP_FILE}.tmp ${_GEN_CPP_FILE}
#    COMMAND ls -l ${_GEN_CPP_FILE}.tmp ${_GEN_CPP_FILE} ${_orig}
     
    DEPENDS ${INFILE}
    COMMENT "Compiling '${INFILE}'..."
    )

  set(${GEN_CPP_FILE} ${_GEN_CPP_FILE} PARENT_SCOPE)
endfunction()

#
#
#
function(check_lex_version GEN_VERSION)
  execute_process(COMMAND ${FLEX} --version
      OUTPUT_VARIABLE CMAKE_FLEX_VERSION)
  string(REGEX REPLACE "flex ([0-9]+)\\.([0-9]+).*$" "\\1"
         CMAKE_FLEX_MAJOR ${CMAKE_FLEX_VERSION})
  string(REGEX REPLACE "flex ([0-9]+)\\.([0-9]+).*" "\\2"
         CMAKE_FLEX_MINOR ${CMAKE_FLEX_VERSION})
  string(REGEX REPLACE "flex ([0-9]+)\\.([0-9]+)\\.([0-9]+).*" "\\3"
         CMAKE_FLEX_PATCH ${CMAKE_FLEX_VERSION})

   set(_GEN_VERSION 0)
   if (${CMAKE_FLEX_MAJOR} EQUAL 2 AND ${CMAKE_FLEX_MINOR} EQUAL 5)
     if(${CMAKE_FLEX_PATCH} GREATER 33)
       set(_GEN_VERSION 1)
     endif(${CMAKE_FLEX_PATCH} GREATER 33)
   endif (${CMAKE_FLEX_MAJOR} EQUAL 2 AND ${CMAKE_FLEX_MINOR} EQUAL 5)

   set(${GEN_VERSION} ${_GEN_VERSION} PARENT_SCOPE)
   message(STATUS "Flex version '${CMAKE_FLEX_MAJOR}' '${CMAKE_FLEX_MINOR}' '${CMAKE_FLEX_PATCH}' ${_GEN_VERSION}")
endfunction()
