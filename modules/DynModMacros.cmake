# -*- mode: cmake; -*-

# Macros used for modgen
function(sing_create_pathname GEN_FILE) 

  message("create pathnames.h...")
#  set(${GEN_FILE} ${INFILE} PARENT_SCOPE)
  set(_GEN_FILE "${CMAKE_CURRENT_BINARY_DIR}/pathnames.h")
#  file(WRITE filename "/*
#*
#*/
#
#define TOPSRCDIR \"$(topsrcdir)\"
#define SINGUNAME \"$(SINGUNAME)\"
#")

  add_custom_command(OUTPUT ${_GEN_FILE}
    COMMAND echo "/\*" > ${_GEN_FILE}
    COMMAND echo " \*" >> ${_GEN_FILE}
    COMMAND echo " \*/" >> ${_GEN_FILE}
    COMMAND echo "" >> ${_GEN_FILE}
    COMMAND echo "#define TOPSRCDIR \"$(topsrcdir)\"" >> ${_GEN_FILE}
    COMMAND echo "#define SINGUNAME \"$(SINGUNAME)\"" >> ${_GEN_FILE}
    COMMENT "Creatting '${_GEN_FILE}'..."
    )
  message(STATUS "argv: ${ARGV}")
  message(STATUS "argn: ${ARGC}")
  message(STATUS "argn: '${ARGN}'")


  set(${GEN_FILE} ${_GEN_FILE} PARENT_SCOPE)
endfunction()

function(sing_create_decl GEN_FILE GRAMMAR_H TOK_H) 

  message("sing_create_decl...")
  set(_GEN_FILE "${CMAKE_CURRENT_BINARY_DIR}/decl.inc")
  add_custom_command(OUTPUT ${_GEN_FILE}
    COMMAND echo "/* declaration for modgen.cc */" > ${GEN_FILE}
    COMMAND grep "^#define" ${GRAMMAR_H} | awk 
    '{print "\tcase "$$2": strcpy(name,\""$$2"\"); break;"}' >> ${_GEN_FILE}
    COMMAND echo "" >> ${_GEN_FILE}
    COMMAND echo "/* declaration from tok.h */" >> ${_GEN_FILE}
    COMMAND grep "^  [A-Z_]*," ../../Singular/tok.h | sed "s/,//g" | awk 
    '{print "\tcase "$$1": strcpy(name,\""$$1"\"); break;"}'>>${_GEN_FILE}
    DEPENDS ${GRAMMAR_H}
    COMMENT "Compiling '${GRAMMAR_H}'..."
    )
  message(STATUS "argv: ${ARGV}")
  message(STATUS "argn: ${ARGC}")
  message(STATUS "argn: '${ARGN}'")


  set(${GEN_FILE} ${_GEN_FILE} PARENT_SCOPE)
endfunction()
