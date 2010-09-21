# -*- mode: cmake; -*-
#
#  omalloc
#  create header
#

## which malloc to use
## OPTION(WITH_MALLOC "system|dlmalloc|gmalloc|pmalloc|external default: malloc")

include(CheckCSourceRuns)

function(omalloc_header GEN_MALLOC_HEADER GEN_MALLOC_SOURCE
			GEN_HAVE_MALLOC_SOURCE
                        GEN_OMALLOC_USES_MALLOC WITH_MALLOC)
  set(_GEN_MALLOC_HEADER omMallocSystem.h)
  set(_GEN_MALLOC_SOURCE "")
  set(_GEN_OMALLOC_USES_MALLOC 0)
message(STATUS "omalloc: '${WITH_MALLOC}'")

  if(${WITH_MALLOC} STREQUAL "system")
    set(_GEN_MALLOC_HEADER omMallocSystem.h)
    set(_GEN_OMALLOC_USES_MALLOC 1)
  elseif(${WITH_MALLOC} STREQUAL "gmalloc")
    set(_GEN_MALLOC_HEADER gmalloc.h)
    set(_GEN_MALLOC_SOURCE gmalloc.c)
  elseif(${WITH_MALLOC} STREQUAL "pmalloc")
    set(_GEN_MALLOC_HEADER pmalloc.h)
    set(_GEN_MALLOC_SOURCE pmalloc.c)
  elseif(${WITH_MALLOC} STREQUAL "external")
  #  set(_GEN_MALLOC_HEADER $with_external_malloc_h)
  #  set(_GEN_MALLOC_SOURCE $with_external_malloc_c)
    set(_GEN_OMALLOC_USES_MALLOC 1)
  elseif(${WITH_MALLOC} STREQUAL "dlmalloc")
    set(_GEN_MALLOC_HEADER dlmalloc.h)
    set(_GEN_MALLOC_SOURCE dlmalloc.c)
    set(_GEN_OMALLOC_USES_MALLOC 1)
  else(${WITH_MALLOC} STREQUAL "system")
    set(_GEN_MALLOC_HEADER omMallocSystem.h)
    set(_GEN_OMALLOC_USES_MALLOC 1)
  endif(${WITH_MALLOC} STREQUAL "system")

  if(NOT {_GEN_MALLOC_SOURCE} EQUAL "")
    set(_GEN_HAVE_MALLOC_SOURCE 1)
  else(NOT {_GEN_MALLOC_SOURCE} EQUAL "")
    set(_GEN_HAVE_MALLOC_SOURCE 0)
  endif(NOT {_GEN_MALLOC_SOURCE} EQUAL "")

  set(${GEN_MALLOC_HEADER} ${_GEN_MALLOC_HEADER} PARENT_SCOPE)
  set(${GEN_MALLOC_SOURCE} ${_GEN_MALLOC_SOURCE} PARENT_SCOPE)
  set(${GEN_HAVE_MALLOC_SOURCE} ${_GEN_HAVE_MALLOC_SOURCE} PARENT_SCOPE)
  set(${GEN_OMALLOC_USES_MALLOC} ${_GEN_OMALLOC_USES_MALLOC} PARENT_SCOPE)

  # check 
  message(STATUS "whether malloc provides SizeOfAddr")
  set(TESTFILE "${CMAKE_CURRENT_BINARY_DIR}/dummy.c")
  set(_TEST_SOURCE "#include \"${_GEN_MALLOC_HEADER}\"
#ifdef OM_HAVE_MALLOC_SOURCE
#include \"${_GEN_MALLOC_SOURCE}\"
#endif

main()
{
  void* addr = OM_MALLOC_MALLOC(512);
  if (OM_MALLOC_SIZEOF_ADDR(addr) < 512)
    exit(1);
  exit(0);
}")

##  file(WRITE ${TESTFILE} "#include \"${${GEN_MALLOC_HEADER}}\"
#ifdef OM_HAVE_MALLOC_SOURCE
#include \"${${GEN_MALLOC_SOURCE}}\"
#endif
##
##main()
##{
##  void* addr = OM_MALLOC_MALLOC(512);
##  if (OM_MALLOC_SIZEOF_ADDR(addr) < 512)
##    exit(1);
##  exit(0);
##}")
##try_run(RUN_RESULT_VAR COMPILE_RESULT_VAR
##          "." ${TESTFILE}
##	  CMAKE_FLAGS -I${CMAKE_CURRENT_SOURCE_DIR} -D__STDC__=1
##)
#          [COMPILE_DEFINITIONS <flags>]
#          [COMPILE_OUTPUT_VARIABLE comp]
#          [RUN_OUTPUT_VARIABLE run]
#          [OUTPUT_VARIABLE var]
#          [ARGS <arg1> <arg2>...])

CHECK_C_SOURCE_RUNS(${_TEST_SOURCE} RUN_RESULT_VAR)
  message(STATUS "compile : '${COMPILE_RESULT_VAR}'")
  message(STATUS "run     : '${RUN_RESULT_VAR}'")
  message(STATUS "rc     : '${_TEST_SOURCE}'")

  add_custom_target(omalloc_header DEPENDS ${GEN_MALLOC_HEADER})
endfunction()

MARK_AS_ADVANCED(
  OM_MALLOC_HEADER
  OM_MALLOC_SOURCE
  OM_HAVE_MALLOC_SOURCE
  OMALLOC_USES_MALLOC
  OM_HAVE_VALLOC_MMAP
  OM_HAVE_VALLOC_MALLOC
)

#
#
#
function(omalloc_create_omtables_inc GEN_TARGET GEN_FILE GENCMD NAME)
  message(STATUS "generate incfile ${NAME}.inc ...")
  set(_GEN_TARGET "${NAME}-inc")
  set(_GEN_FILE   ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.inc)

  add_custom_command(
    OUTPUT ${_GEN_FILE}
    COMMAND ${GENCMD} > ${_GEN_FILE}
    DEPENDS ${GENCMD}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Creating '${NAME}.inc'..."
    )

  set(${GEN_FILE}   ${_GEN_FILE}   PARENT_SCOPE)
  set(${GEN_TARGET} ${_GEN_TARGET} PARENT_SCOPE)
endfunction()

#
#
#
function(omalloc_create_omtables_h GEN_TARGET GEN_H_FILE GENCMD NAME)
  message(STATUS "generate header ${NAME}.h ...")
  set(_GEN_TARGET "gen-header-${NAME}")
  set(_GEN_H_FILE ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.h)

  add_custom_command(
    OUTPUT ${_GEN_H_FILE}
    COMMAND ${GENCMD} 1 > ${_GEN_H_FILE}
    DEPENDS ${GENCMD}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Creating '${NAME}.h'..."
    )

  set(${GEN_H_FILE} ${_GEN_H_FILE} PARENT_SCOPE)
  set(${GEN_TARGET} ${_GEN_TARGET} PARENT_SCOPE)
endfunction()
