# -*- mode: cmake; -*-
###
## Optionen
## c++ -I../include -I.  -O2 -fomit-frame-pointer -fno-rtti -c ZZ_pEX.c
##
project(ntl)
cmake_minimum_required(VERSION "2.6" FATAL_ERROR)

set(V_MAJOR 5)
set(V_MINOR 5)
set(V_PATCH RC1)

set(PROJECT_DESCRIPTION "NTL  -- a library for doing numbery theory")
set(PROJECT_VENDOR      "")
set(PROJECT_COPYRIGHT   "GNU General Public License")
set(PROJECT_CONTACT     "Victor Shoup (victor@shoup.net)")
set(PROJECT_VERSION     "${V_MAJOR}.${V_MINOR}.${V_PATCH}")
set(PROJECT_README      "${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}-${V_MAJOR}.${V_MINOR}/README")
set(PROJECT_COPYING     "${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}-${V_MAJOR}.${V_MINOR}/COPYING")

# have the full monty in makefiles
set(CMAKE_VERBOSE_MAKEFILE true)

OPTION(WIZARD "Run wizard" off)

###################################################################################################
# No user settings below
###################################################################################################
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")



set(CC gcc)
set(CXX g++)
set(NTL_GMP_LIP On)
set(NTL_STD_CXX On)
set(GMP_PREFIX /)

set(CMAKE_C_FLAGS "-O2 -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS "-O2 -fomit-frame-pointer -fno-rtti")
set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -Wno-non-virtual-dtor -Wno-unknown-pragma")

link_libraries(m)
set(CMAKE_INSTALL_PREFIX "/usr/local")

set(GMP_PREFIX ${CMAKE_INSTALL_PREFIX})
set(GMP_INCDIR ${CMAKE_INSTALL_PREFIX}/include)
set(GMP_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)

set(GF2X_PREFIX ${CMAKE_INSTALL_PREFIX})
set(GF2X_INCDIR ${CMAKE_INSTALL_PREFIX}/include)
set(GF2X_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)

set(NTL_STD_CXX   on)
set(NTL_PSTD_NNS  off)
set(NTL_PSTD_NHF  off)
set(NTL_PSTD_NTN  off)

set(NTL_GMP_LIP   on)
set(NTL_GMP_HACK  off)

set(NTL_GF2X_LIB  off)
set(NTL_GF2X_NOINLINE  off)
set(NTL_GF2X_ALTCODE   off)
set(NTL_GF2X_ALTCODE1  off)

set(NTL_CXX_ONLY  off)
set(NTL_X86_FIX     off)
set(NTL_NO_X86_FIX  off)
set(NTL_AVOID_FLOAT off)
set(NTL_LONG_LONG   off)

set(NTL_SPMM_ULL    off)
set(NTL_SPMM_UL     off)
set(NTL_SPMM_ASM    off)

set(NTL_AVOID_BRANCHING  off)
set(NTL_TBL_REM        off)
set(NTL_NO_INIT_TRANS  off)
set(NTL_CLEAN_INT      off)
set(NTL_CLEAN_PTR      off)
set(NTL_RANGE_CHECK    off)
set(NTL_LONG_LONG_TYPE 0)
set(NTL_UNSIGNED_LONG_LONG_TYPE 0)

if(${NTL_GMP_LIP})
  include(FindGMP)
endif(${NTL_GMP_LIP})

if(${NTL_GF2X_LIB})
  include(FindGF2X)
endif()

# copy file DoCOnfig
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/cfile
  ${CMAKE_CURRENT_BINARY_DIR}/cfile COPYONLY
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/DoConfig
  ${CMAKE_CURRENT_BINARY_DIR}/DoConfig COPYONLY
)

#add_custom_command(
execute_process(
#  OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/cfileout
  COMMAND perl ${CMAKE_CURRENT_BINARY_DIR}/DoConfig NTL_GMP_LIP=on GMP_PREFIX=/ CFLAGS=" -O2 -fomit-frame-pointer" CXXFLAGS=" -O2 -fomit-frame-pointer -fno-rtti" LDFLAGS="" NTL_STD_CXX=off
  COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/include/NTL
  COMMAND cp cfileout ${CMAKE_CURRENT_BINARY_DIR}/include/NTL/config.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#  COMMENT "Creating cfileout"
)
#configure_file(${CMAKE_CURRENT_BINARY_DIR}/src/cfileout
#  ${CMAKE_CURRENT_BINARY_DIR}/include/NTL/config.hss COPYONLY
#)

#configure_file(${CMAKE_CURRENT_BINARY_DIR}/cfileout
#  ${CMAKE_CURRENT_BINARY_DIR}/include/NTL/config.h)

#execute_process(COMMAND <cmd1> [args1...]]
#                  [COMMAND <cmd2> [args2...] [...]]
#                  [WORKING_DIRECTORY <directory>]
#                  [TIMEOUT <seconds>]
#                  [RESULT_VARIABLE <variable>]
#                  [OUTPUT_VARIABLE <variable>]
#                  [ERROR_VARIABLE <variable>]
#                  [INPUT_FILE <file>]
#                  [OUTPUT_FILE <file>]
#                  [ERROR_FILE <file>]
#                  [OUTPUT_QUIET]
#                  [ERROR_QUIET]
#                  [OUTPUT_STRIP_TRAILING_WHITESPACE]
#                  [ERROR_STRIP_TRAILING_WHITESPACE])

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
add_subdirectory(src)


