# -*- mode: cmake; -*-
cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)
if(POLICY CMP0011)
  cmake_policy(SET CMP0011 NEW)
endif(POLICY CMP0011)

project(libfac)
string(TOUPPER ${PROJECT_NAME} UPPER_PROJECT_NAME)

# have the full monty in makefiles
set(CMAKE_VERBOSE_MAKEFILE true)

set(LIBFAC_V_MAJOR 3)
set(LIBFAC_V_MINOR 1)
set(LIBFAC_V_PATCH 0)
set(LIBFAC_NAME   "Factorization and characteristic sets library")
set(LIBFAC_DATE   "Aug 2008")
set(LIBFAC_AUTHOR "M. Messollen")

#OPTION(WITH_SINGULAR "build for use with Singular" Off)
#OPTION(WITH_SINGULARERROR "use error reporting from Singular" Off)
OPTION(WITH_NOSTREAMIO    "do not require iostream include nor library" Off)
set(ADD_DEFS "NOSTREAMIO;${_OSDEF};HAVE_CONFIG_H;SINGULAR")


###################################################################################################
# No user settings below
###################################################################################################
include(LibfacMacros)

###################################################################################################
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  message(STATUS "Have compiler: '${CMAKE_CXX_COMPILER_ID}'")
  if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")

    #set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -Wno-non-virtual-dtor")
    set(CMAKE_CXX_FLAGS "-w --no-rtti -pipe ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-implicit-templates")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fomit-frame-pointer -DNDEBUG")

    set(CMAKE_CXX_FLAGS_DEBUG "-Werror -O0 -g -ggdb -Wreturn-type -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wunused-variable -Wunused-parameter")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wunused-function -Wunused")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Woverloaded-virtual -Wno-system-headers")

    # gprof and gcov support
    set(CMAKE_CXX_FLAGS_PROFILE "-O0 -g -ggdb -Wreturn-type -Woverloaded-virtual")
    set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -Wno-system-headers -pg")
    set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -fprofile-arcs -ftest-coverage")
  endif (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

###########################################################
# Where are the additional libraries installed? Note: provide includes
# path here, subsequent checks will resolve everything else
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/charset)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/factor)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR}/factory)
include_directories(${CMAKE_SOURCE_DIR}/factory)

libfac_mk_version(LIBFAC_VERSION_H)


##
## src and objects
##

set(FACTORSRC1  factor/SqrFree.cc factor/Factor.cc 
		factor/MVMultiHensel.cc factor/Truefactor.cc 
		factor/helpstuff.cc  factor/version.cc 
		factor/homogfactor.cc )
set(FACTORSRC2 	factor/tmpl_inst.cc)
set(FACTORSRC  	${FACTORSRC2} ${FACTORSRC1})


set(FACTORINCL 	${LIBFAC_VERSION_H} 
  factor/Factor.h factor/MVMultiHensel.h
  factor/Truefactor.h factor/tmpl_inst.h factor/SqrFree.h
  factor/helpstuff.h factor/debug.h factor/timing.h 
  factor/homogfactor.h     factor/interrupt.h)
set(CHARSETINCL charset/csutil.h charset/charset.h charset/reorder.h
  charset/alg_factor.h charset/algfactor.h)

set(charset_srcs  charset/csutil.cc charset/charset.cc
  charset/reorder.cc charset/alg_factor.cc charset/algfactor.cc
)

# define libraries
add_library(fac ${LIBFAC_VERSION_H} ${FACTORSRC} ${CHARSETSRC})
set_source_files_properties(${FACTORSRC} PROPERTIES OBJECT_DEPENDS 
  ${FACTORYCONF_H})
#add_dependencies(fac ${FACTORY_TARGET})

# debug library
#add_library(fac_g ${LIBFAC_VERSION_H} ${FACTORSRC} ${CHARSETSRC})
#set_target_properties(fac_g PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})

# profile library
#add_library(fac_p ${LIBFAC_VERSION_H} ${FACTORSRC} ${CHARSETSRC})
#set_target_properties(fac_p PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS_PROFILE})

#
# libraries for Singular
if( WITH_SINGULAR )
  message(STATUS "Compiling libfac for Singular: creating libsingfac.a")
  add_library(singfac ${LIBFAC_VERSION_H} ${FACTORSRC1} ${charset_srcs})
  set_target_properties(singfac PROPERTIES 
    COMPILE_DEFINITION "${ADD_DEFS};SINGULAR")

#  # debug library
#  add_library(singfac_g ${LIBFAC_VERSION_H} ${FACTORSRC1} ${charset_srcs})
#  set_target_properties(singfac_g PROPERTIES COMPILE_FLAGS "-DSINGULAR ${CMAKE_CXX_FLAGS_DEBUG}")

#  # profile library
#  add_library(singfac_p ${LIBFAC_VERSION_H} ${FACTORSRC1} ${charset_srcs})
#  set_target_properties(singfac_p PROPERTIES COMPILE_FLAGS "-DSINGULAR ${CMAKE_CXX_FLAG_PROFILE}")
endif( WITH_SINGULAR )
