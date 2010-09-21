# -*- mode: cmake; -*-
project(omalloc)

# have the full monty in makefiles
set(CMAKE_VERBOSE_MAKEFILE true)

set(V_MAJOR 1)
set(V_MINOR 24)
set(V_PATCH RC1)

set(WITH_MALLOC "system")

OPTION(WITH_EXTERNAL_CONFIG_H "use HEADER_FILE for external configuration")
OPTION(WITH_EXTERNAL_config_c "use C_FILE for external implementations")
OPTION(WITH_MALLOC "system|dlmalloc|gmalloc|pmalloc|external which malloc to use, default: malloc")
OPTION(WITH_EXTERNAL_MALLOC_H "use HEADER_FILE for external malloc declaration")
OPTION(WITH_EXTERNAL_MALLOC_C "use C_FILE for external malloc implementation")
OPTION(WITH_VALLOC "mmap|system|emulate how to get page-aligned memory, default: use first possible")
OPTION(WITH_PROVIDE_MALLOC "yes|debug|malloc provide normal|debug|underlying malloc ANSI-C conforming versions of malloc|calloc|realloc|free")
OPTION(WITH_EMULATE_OMALLOC "provide only omallocs interface, but do not use its features")
OPTION(WITH_ALIGN "8|sloppy|strict how memory is aligned, default: if possible sloppy, else strict")
OPTION(WITH_DENSE_BINS "use dense bin distribution")
OPTION(WITH_INLINE "do not inline")
OPTION(WITH_DEBUG "disable all debugging facilities")
OPTION(WITH_TRACK "disable debug tracking functionality")
OPTION(WITH_TRACK_FL "track file and line numbers")
OPTION(WITH_TRACK_RETURN "track return addresses")
OPTION(WITH_TRACK_BACKTRACE "track stack backtraces")
OPTION(WITH_TRACK_CUSTOM "track custom values")
OPTION(WITH_INTERNAL_DEBUG "turn on internal debugging")

set(EXTERNAL_CONFIG_SOURCE "")
set(EXTERNAL_CONFIG_HEADER "")

###########################################################
# add a path where some libraries might be stored
set(CMAKE_ADDITIONAL_PATH "$ENV{CMAKE_ADDITIONAL_PATH}"
    CACHE PATH "Path where many locally installed libraries can be found")

###########################################################
# No user settings below
###########################################################
cmake_minimum_required(VERSION "2.6" FATAL_ERROR)

# check if compiled standalone
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
  set(OM_HAVE_EXTERNAL_CONFIG_H 0)
  set(OM_TRACK_CUSTOM 0)

else(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

  set(EXTERNAL_CONFIG_HEADER "${CMAKE_SOURCE_DIR}/Singular/omSingularConfig.h")
  set(OM_HAVE_EXTERNAL_CONFIG_H 1)
  set(OM_TRACK_CUSTOM 1)
  message(STATUS "OM_HAVE_EXTERNAL_CONFIG_H='${OM_HAVE_EXTERNAL_CONFIG_H}'")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

#
set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS "")

set(CMAKE_C_FLAGS_RELEASE "-g -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 -DNDEBUG")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g -Wreturn-type -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-inline-functions")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -W -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wreturn-type -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-inline-functions")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -W -Wall -Wextra")

set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_DEBUG} -Werror -pedantic")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_DEBUG} -Werror -pedantic")

include(FindHeader)

check_headers(limits.h)
check_headers(unistd.h sys/mman.h fcntl.h malloc.h)
set(HAVE_FCNTL_H 1)
set(HAVE_SYS_MMAN_H 1)

include(CheckFunctionExists)
check_funcs(popen readlink getcwd getwd mmap sbrk random)

include (CheckTypeSize)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)
check_type_size(void* SIZEOF_VOIDP)
check_type_size(double SIZEOF_DOUBLE)
check_type_size(size_t SIZEOF_SIZE_T)

if(NOT ${SIZEOF_LONG} EQUAL ${SIZEOF_VOIDP})
  message(FATAL_ERROR "need equal sizes for long and void*")
endif(NOT ${SIZEOF_LONG} EQUAL ${SIZEOF_VOIDP})

if(NOT ${SIZEOF_VOIDP} EQUAL 4)
  if(NOT ${SIZEOF_VOIDP} EQUAL 8)
    message(FATAL_ERROR "need void* to be 4 or 8 bytes long")
  endif(NOT ${SIZEOF_VOIDP} EQUAL 8)
endif(NOT ${SIZEOF_VOIDP} EQUAL 4)

if(NOT ${SIZEOF_DOUBLE} EQUAL 4)
  if(NOT ${SIZEOF_DOUBLE} EQUAL 8)
    message(FATAL_ERROR "need double* to be 4 or 8 bytes long")
  endif(NOT ${SIZEOF_DOUBLE} EQUAL 8)
endif(NOT ${SIZEOF_DOUBLE} EQUAL 4)

## which malloc to use
## OPTION(WITH_MALLOC "system|dlmalloc|gmalloc|pmalloc|external default: malloc")
include(CreateOmallocHeader)
include(OMallocMacros)

set(OM_INLINE_DECL "static inline")
set(OM_INLINE_LOCAL "static inline")
set(OM_INLINE_IMPL "static inline")
set(OM_INLINE "static inline")
set(OM_PROG_ADDR2LINE "addr2line")
set(HAVE_WORKING_MMAP Yes)
set(STDC_HEADERS Yes)
set(OM_GET_RETURN_ADDR_WORKS Yes)
set(OM_GET_BACKTRACE_WORKS 0)
set(OM_HAVE_VALLOC_MMAP 1)
set(OM_HAVE_VALLOC_MALLOC 0)
set(OM_ALIGN_8 1)
set(OM_TRACK_FILE_LINE 0)
set(OM_HAVE_TRACK 1)
set(OM_TRACK_RETURN 1)
set(OM_TRACK_BACKTRACE 0)
set(OM_INTERNAL_DEBUG 0)


include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})

###
omalloc_header(OM_MALLOC_HEADER OM_MALLOC_SOURCE
               OM_HAVE_MALLOC_SOURCE OMALLOC_USES_MALLOC
               ${WITH_MALLOC})
message(STATUS "Malloc header        : '${OM_MALLOC_HEADER}'")
message(STATUS "Malloc source        : '${OM_MALLOC_SOURCE}'")
message(STATUS "OMALLOC_USES_MALLOC  : '${OMALLOC_USES_MALLOC}'")
message(STATUS "OM_HAVE_MALLOC_SOURCE: '${OM_HAVE_MALLOC_SOURCE}'")

###
### configure files
###
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/omConfig.h.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/omConfig.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/omlimits.h.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/omlimits.h)
configure_file(${OM_MALLOC_HEADER}
  ${CMAKE_CURRENT_BINARY_DIR}/omMalloc.h)

if( NOT ${EXTERNAL_CONFIG_HEADER} STREQUAL "" )
  message(STATUS "=============2")
  configure_file(${EXTERNAL_CONFIG_HEADER}
    ${CMAKE_CURRENT_BINARY_DIR}/omExternalConfig.h COPYONLY)
endif( NOT ${EXTERNAL_CONFIG_HEADER} STREQUAL "" )

###
### compile and Create omTables
###
set(omTables_srcs omAllocPrivate.h
  ${CMAKE_CURRENT_BINARY_DIR}/omConfig.h omTables.c
  ${CMAKE_CURRENT_BINARY_DIR}/omlimits.h mylimits.h)
add_executable(omTables ${omTables_srcs})
set_target_properties(omTables PROPERTIES
  COMPILE_DEFINIIONS "HAVE_CONFIG_H;OM_GENERATE_INC")

omalloc_create_omtables_inc(OMTABLES_TARGET OMTABLES_INC
  ${CMAKE_CURRENT_BINARY_DIR}/omTables omTables)

omalloc_create_omtables_h(TARGET_OMTABLES OMTABLES_H
  ${CMAKE_CURRENT_BINARY_DIR}/omTables omTables)

set(OMALLOC_H "${CMAKE_CURRENT_BINARY_DIR}/omalloc.h")
omalloc_create_omalloc_h(gen-omalloc-h ${OMALLOC_H}
  ${CMAKE_CURRENT_SOURCE_DIR}/makeheader
  ${CMAKE_CURRENT_SOURCE_DIR}/om_Alloc.h ${OMTABLES_H} ${TARGET_OMTABLES})
set(OMALLOC_H ${OMALLOC_H} PARENT_SCOPE)

message(STATUS "Header file omalloc: '${OMALLOC_H}'")
set_source_files_properties(omalloc.c PROPERTIES OBJECT_DEPENDS ${OMALLOC_H})
set_source_files_properties(omalloc_debug.c PROPERTIES OBJECT_DEPENDS ${OMALLOC_H})
set_source_files_properties(omAllocFunc.c PROPERTIES OBJECT_DEPENDS ${OMALLOC_H})
set_source_files_properties(omalloc_provide.c PROPERTIES OBJECT_DEPENDS ${OMALLOC_H})

###########################################################
# set variables with source files first
set(omalloc_c_srcs
  omBinPage.c omList.c omAllocEmulate.c
  omDebug.c om_Alloc.c omDebugCheck.c omOpts.c omGetBackTrace.c
  omAllocSystem.c omError.c omStats.c omRet2Info.c  omBin.c  omFindExec.c
  omDebugTrack.c  omalloc_provide.c omAllocFunc.c
  )

set(omalloc_hdrs
  ${CMAKE_CURRENT_BINARY_DIR}/omlimits.h
  ${CMAKE_CURRENT_BINARY_DIR}/omConfig.h
  mylimits.h om_Alloc.h omList.h omDerivedConfig.h omAllocDecl.h
  omMemOps.h omDebug.h omOpts.h omInlineDecl.h omAllocPrivate.h
  omError.h omPage.h omAllocSystem.h omFindExec.h omStats.h omBin.h
  omDefaultConfig.h omStructs.h omBinPage.h omInline.h omGetBackTrace.h
  omReturn.h omRet2Info.h omAllocFunc.h)

set(omalloc_srcs ${omalloc_c_srcs} omGetPageSize.h omMallocSystem.h
# omMmap.c omTables.c
#gmalloc.c   pmalloc.h pmalloc.c omalloc.c omalloc_debug.c
#omtTestAlloc.c
  ${TESTSOURCES}  ${omalloc_hdrs})

if( EXTERNAL_CONFIG_SOURCE)
  set(GSOURCES ${GSOURCES} omExternalConfig.c)
endif( EXTERNAL_CONFIG_SOURCE)

if(${OM_MALLOC_SOURCE})
  set(GSOURCES ${GSOURCES} omMalloc.c)
endif(${OM_MALLOC_SOURCE})

message(STATUS "omalloc_srcs='${omalloc_srcs}'")

#set(_OMALLOC_TARGET "gen-omalloc")
#add_custom_target(${_OMALLOC_TARGET} DEPENDS ${OMALLOC_H})
#set(OMALLOC_TARGET ${_OMALLOC_TARGET} PARENT_SCOPE)
#set_source_files_properties(${OMALLOC_H} GENERATED)

#set_source_files_properties(${omalloc_c_srcs} PROPERTIES OBJECT_DEPENDS ${OMALLOC_H})

###########################################################

###########################################################

###SET_SOURCE_FILES_PROPERTIES(omalloc.c OBJECT_DEPENDS ${OMALLOC_H})



set(TESTSOURCES "")
# gmalloc.c
# dlmalloc.h dlmalloc.c

# define libraries

add_library(omalloc ${OMTABLES_INC} ${OMTABLES_H} ${omalloc_srcs})
set_target_properties(omalloc PROPERTIES
  COMPILE_DEFINITIONS HAVE_CONFIG_H)
add_dependencies(omalloc ${TARGET_OMTABLES})
add_dependencies(omalloc gen-omalloc-h)

add_library(omalloc_ndebug ${OMALLOC_H} ${OMTABLES_INC} ${OMTABLES_H}
  ${omalloc_srcs})
set_target_properties(omalloc_ndebug PROPERTIES
  COMPILE_DEFINITIONS "HAVE_CONFIG_H;OM_NDEBUG")

add_library(omalloc-small ${OMALLOC_H} omalloc.c)
set_target_properties(omalloc-small PROPERTIES
  COMPILE_DEFINITIONS HAVE_CONFIG_H)

add_library(omalloc-small_ndebug ${OMALLOC_H} omalloc.c)
set_target_properties(omalloc-small_ndebug PROPERTIES
  COMPILE_DEFINITIONS "HAVE_CONFIG_H;OM_NDEBUG")

###########################################################
## Install
install(TARGETS omalloc omalloc_ndebug omalloc-small omalloc-small_ndebug
  ARCHIVE DESTINATION lib
)
install(FILES ${OMALLOC_H} omalloc.c mylimits.h
  DESTINATION include/Singular/omalloc
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/omlimits.h
  DESTINATION include/Singular/omalloc
)
