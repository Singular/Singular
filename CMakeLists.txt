# -*- mode: cmake; -*-
project(Singular)
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)
cmake_policy(VERSION 2.6)
#if(POLICY CMP0011)
#  cmake_policy(SET CMP0011 NEW)
#endif(POLICY CMP0011)

# have the full monty in makefiles
set(CMAKE_VERBOSE_MAKEFILE true)

set(V_MAJOR 3)
set(V_MINOR 1)
set(V_PATCH 0)
set(VERSION_SEP "-")

set(SINGULAR_SHORT_VERSION "${V_MAJOR}${VERSION_SEP}${V_MINOR}")
set(SINGULAR_VERSION "${SINGULAR_SHORT_VERSION}${VERSION_SEP}${V_PATCH}")
set(SINGULAR_RPM_VERSION "${V_MAJOR}.${V_MINOR}.${V_PATCH}")
set(VERSION_DATE "2010-01-12")
set(SINGULAR_ROOT_DIR {${PROJECT_NAME}_SOURCE_DIR}) # is ${top_srcdir}

set(PROJECT_DESCRIPTION "A Computer Algebra System for Polynomial Computations")
set(PROJECT_VENDOR      "FB Mathematik der Universitaet, D-67653 Kaiserslautern")
set(PROJECT_COPYRIGHT   "Copyright 2009 ")
set(PROJECT_CONTACT     "<singular@mathematik.uni-kl.de>")
set(PROJECT_VERSION     "${V_MAJOR}.${V_MINOR}.${V_PATCH}")
set(PROJECT_README      "${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}-${V_MAJOR}.${V_MINOR}/README")
set(PROJECT_COPYING     "${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}-${V_MAJOR}.${V_MINOR}/COPYING")
set(PROJECT_DIR         "${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}")

###########################################################
# tree of dependencies (attempt)
#
# omalloc    ntl
#     \      /
#     factory
#
# Singular
# kernel
# libfac

###########################################################
# we do not want to run cmake in the source directory
if( ${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
   message(STATUS "DO not run cmake in the source directory")
   message(STATUS "create an extra binary directory")
   message(STATUS "Don't forget to remove CMakeCache.txt CMakeFiles")
   message(FATAL_ERROR "Exiting cmake here.")
endif( ${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

#message(FATAL_ERROR "my test failed")

###########################################################
# add a path where some libraries might be stored
set(CMAKE_ADDITIONAL_PATH "$ENV{CMAKE_ADDITIONAL_PATH}" 
    CACHE PATH "Path where many locally installed libraries can be found")

###########################################################
# Where are the additional libraries installed? Note: provide includes
# path here, subsequent checks will resolve everything else
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${CMAKE_ADDITIONAL_PATH}/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CMAKE_ADDITIONAL_PATH}/lib)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
  "${CMAKE_SOURCE_DIR}/modules" 
  "$ENV{CMAKE_MODULE_PATH}")

###########################################################
##
###########################################################
OPTION(ENABLE_OMALLOC   "omalloc memory managment" ON)
OPTION(ENABLE_GMP       "Gnu Multiple Precision package" ON)
OPTION(ENABLE_FACTORY   "polynomial factorization library" ON)
OPTION(ENABLE_LIBFAC    "char set and modp poly factorization library" ON)
OPTION(ENABLE_SINGULAR  "CAS for Polynomial Computations" ON)
OPTION(ENABLE_MP        "Multi Protocol library" OFF)

###########################################################
##
###########################################################
set(ENABLE_OMALLOC  On)
set(ENABLE_GMP      On)
set(ENABLE_FACTORY  On)
set(ENABLE_LIBFAC   On)
set(ENABLE_SINGULAR On)
set(ENABLE_NTL      On)

###########################################################
##
###########################################################
#set(STDC_HEADERS 1)
set(NOSTREAMIO 0)

set(NTL_TARGET gen-gen)

## just for test
message(STATUS "CMAKE_SYSTEM_PROCESSOR='${CMAKE_SYSTEM_PROCESSOR}'")
message(STATUS "CMAKE_SYSTEM='${CMAKE_SYSTEM}'")
message(STATUS "CMAKE_SYSTEM_NAME='${CMAKE_SYSTEM_NAME}'")
message(STATUS "CMAKE_SYSTEM_VERSION='${CMAKE_SYSTEM_VERSION}'")
message(STATUS "PROJECT_DIR='${PROJECT_DIR}'")

# OSDEF can be: x86_64_Linux
set(_OSDEF "NDEBUG;OM_NDEBUG;${CMAKE_SYSTEM_PROCESSOR}_${CMAKE_SYSTEM_NAME}")
#set(ADD_DEFS "NOSTREAMIO;${_OSDEF};HAVE_CONFIG_H;SINGULAR")
set(ADD_DEFS "${_OSDEF};HAVE_CONFIG_H")

###########################################################
# add a path where some libraries might be stored
set(CMAKE_ADDITIONAL_PATH "$ENV{CMAKE_ADDITIONAL_PATH}"
    CACHE PATH "Path where many locally installed libraries can be found")

###########################################################
# No user settings below
###########################################################
message(STATUS "Have compiler: '${CMAKE_CXX_COMPILER_ID}'")
if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
  #if (${CMAKE_BUILD_TYPE} MATCHES "Release")
  #endif (${CMAKE_BUILD_TYPE} MATCHES "Release")

  #set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -Wno-non-virtual-dtor")
  set(CMAKE_C_FLAGS "-w -pipe ")
  set(CMAKE_CXX_FLAGS "-w  -pipe ") #--no-rtti
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-implicit-templates")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

  set(CMAKE_C_FLAGS_RELEASE "-O3 -fomit-frame-pointer")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 --no-rtti -fomit-frame-pointer")

  #set(CMAKE_C_FLAGS_RELEASE "-O2 -fomit-frame-pointer -DNDEBUG ")
  #set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fomit-frame-pointer -DNDEBUG -DOM_NDEBUG")

  #set(CMAKE_CXX_FLAGS_DEBUG "-Werror -O0 -g -ggdb -Wreturn-type -fno-omit-frame-pointer")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wreturn-type -fno-omit-frame-pointer")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wunused-variable -Wunused-parameter")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wunused-function -Wunused")
  #set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Woverloaded-virtual")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-system-headers")

  #set(CMAKE_C_FLAGS_DEBUG "-Werror -O0 -g -ggdb -Wreturn-type -fno-omit-frame-pointer")
  set(CMAKE_C_FLAGS_DEBUG "-O0 -g -Wreturn-type -fno-omit-frame-pointer")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-inline-functions")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wunused-variable -Wunused-parameter")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wunused-function -Wunused")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wno-system-headers -Wall")

  # gprof and gcov support
  set(CMAKE_CXX_FLAGS_PROFILE "-O0 -g -ggdb -Wreturn-type -Woverloaded-virtual")
#  set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -Wno-system-headers -pg")
  set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -Wno-system-headers ")
  set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -fprofile-arcs -ftest-coverage")

  set(CMAKE_C_FLAGS_PROFILE "-O0 -g -ggdb -Wreturn-type -Woverloaded-virtual")
  set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_PROFILE} -Wno-system-headers -pg")
  set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_PROFILE} -fprofile-arcs -ftest-coverage")
endif (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")

###########################################################
# Where are the additional libraries installed? Note: provide includes
# path here, subsequent checks will resolve everything else
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${CMAKE_ADDITIONAL_PATH}/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CMAKE_ADDITIONAL_PATH}/lib)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

###########################################################
# load requires cmake includes
include(LibMacros)

###########################################################
### check programms
###########################################################
find_program(PERL_CMD perl)
find_program(UUDECODE uudecode)
find_program(GUNZIP gunzip)
find_program(GZIP gzip)
find_program(LATEX2HTML latex2html)

###########################################################
### check libraries
###########################################################
include(InstallRequiredSystemLibraries)


###########################################################
### check header files
###########################################################
include(FindHeader)
check_headers(gmp.h MP.h  MPT.h factory.h factor.h omalloc.h)
check_headers(limits.h unistd.h)
check_headers(sys/file.h sys/ioctl.h sys/time.h sys/times.h sys/types.h
  sys/stat.h fcntl.h sys/param.h pwd.h asm/sigcontext.h pwd.h termcap.h
  termios.h term.h readline/readline.h readline/history.h)
check_headers(iostream.h iostream)

check_library(m)
check_library(bsd)
check_library(socket)
check_library(nsl)
check_library(gmp)
check_library(MPT)
check_library(readline)
set(HAVE_LIBREADLINE 1)
set(HAVE_READLINE 1)
check_library(dl)
if(${HAVE_LIBDL})
  message(STATUS "setting dynamic kernel to yes")
  set(DL_KERNEL 1)
endif(${HAVE_LIBDL})

include(CheckFunctionExists)
check_funcs(popen readlink sbrk getcwd getwd)
check_funcs(gettimeofday atexit bcopy getcwd getwd vsnprintf readlink
  sleep usleep getpwnam popen setenv siginterrupt sysconf)
set(TIME_WITH_SYS_TIME 1)

###########################################################
include (CheckTypeSize)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)
check_type_size(void* SIZEOF_VOIDP)
check_type_size(double SIZEOF_DOUBLE)
check_type_size(size_t SIZEOF_SIZE_T)


###########################################################
## set include directories
#include_directories(${CMAKE_CURRENT_BINARY_DIR})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/factory)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ntl/include)

## PkgConfig is helpful
include(FindPkgConfig)
if ( PKG_CONFIG_FOUND )
#   pkg_check_modules (CPPUNIT REQUIRED cppunit>=1.12)
#   message(STATUS "CPPUnit: -I${CPPUNIT_INCLUDE_DIRS} -L${CPPUNIT_LIBRARY_DIRS} -l${CPPUNIT_LIBRARIES}")

else  ( PKG_CONFIG_FOUND )
  message(FATAL_ERROR "No PKGCONFIG found - hoping for the best.")
  IF(WIN32)
    # search packages by hand
    # CPPunit
    include(FindCppUnit)
  ENDIF(WIN32)
endif ( PKG_CONFIG_FOUND )

if( ${CMAKE_BUILD_TYPE} MATCHES "Release")
  set(MAKE_DISTRIBUTION 1)
endif( ${CMAKE_BUILD_TYPE} MATCHES "Release")
message(STATUS ">>${CMAKE_BUILD_TYPE}<<")
message(STATUS ">>${MAKE_DISTRIBUTION}<<")

###########################################################
### check directories
###########################################################
set(TMPDIR ${${PROJECT_NAME}_BINARY_DIR}/tmp)
# set various define used with configure. 
if(ENABLE_SINGULAR)
  SET(WITH_SINGULAR On)
  set(WITH_SINGULARERROR On)
endif(ENABLE_SINGULAR)

# copied from configure, seems to be useless, because omalloc is now part 
# of the project
if(ENABLE_OMALLOC)
  add_subdirectory(omalloc)
else(ENABLE_OMALLOC)
  message(FATAL_ERROR "can not build without omalloc")
endif(ENABLE_OMALLOC)

message(STATUS "Header 0: '${OMALLOC_H}'")
if(ENABLE_NTL)
  include(NTLMacros)
  add_ntl(NTL_TARGET NTL_LIBRARY ${CMAKE_SOURCE_DIR}/ntl)
  include_directories(${CMAKE_SOURCE_DIR}/ntl/include)
  message(STATUS "Compiling with ntl target '${NTL_TARGET}'")
else(ENABLE_NTL)
  message(STATUS "Compiling without ntl")
endif(ENABLE_NTL)

# switch ENABLE_FACTORY ist useless. Singular does not compile without
# factory. 
if(ENABLE_FACTORY)
  set(HAVE_FACTORY 1)
  add_subdirectory(factory)
  message(STATUS "Compiling with factory")
else(ENABLE_FACTORY)
  message(STATUS "Singular canot be compiled without factory.")
endif(ENABLE_FACTORY)

if(ENABLE_LIBFAC)
  set(HAVE_LIBFAC 0)
  add_subdirectory(libfac)
  add_dependencies(fac ${FACTORY_TARGET})
  message(STATUS "Compiling with libfac")
else(ENABLE_LIBFAC)
  message(STATUS "Compiling without libfac")
endif(ENABLE_LIBFAC)

# MP Multi-Protocol is not 64bit clean. 
include (CheckTypeSize)
check_type_size(long SIZEOF_LONG)
message(STATUS "Sizeof long is ${SIZEOF_LONG}.")
if( NOT ${SIZEOF_LONG} EQUAL 4)
  message(STATUS "MP disabled!")
#  set(ENABLE_MP No)
endif(NOT ${SIZEOF_LONG} EQUAL 4)
if(ENABLE_MP)
  if(ENABLE_GMP)
    add_subdirectory(MP)
    message(STATUS "Compile with local  MP.")
    set(WITH_MP Yes)
  else(ENABLE_GMP)
    set(ENABLE_MP No)
    set(WITH_MP No)
    message(STATUS "can not build MP without gmp.")
  endif(ENABLE_GMP)
endif(ENABLE_MP)

add_subdirectory(kernel)
add_subdirectory(Singular)

include(CTest)
enable_testing()
#add_subdirectory(tests)

# print configuration
message(STATUS "Report of the settings used")
message(STATUS "Enable Singular is .........: ${ENABLE_SINGULAR}")
message(STATUS "Enable omalloc is ..........: ${ENABLE_OMALLOC}")
message(STATUS "Enable gmp is ..............: ${ENABLE_GMP}")
message(STATUS "Enable MP is ...............: ${ENABLE_MP}")
message(STATUS "Enable factory .............: ${ENABLE_FACTORY}")
message(STATUS "Enable libfac ..............: ${ENABLE_LIBFAC}")
message(STATUS "Enable .....................: ${ENABLE_SINGULAR}")
message(STATUS "Enable TexInfo .............: ${ENABLE_TEXINFO}")
message(STATUS "Enable Texi2Html ...........: ${ENABLE_TEXI2HTML}")
message(STATUS "Enable doc .................: ${ENABLE_DOC}")
message(STATUS "Enable emacs ...............: ${ENABLE_EMACS}")

#message(STATUS "with MP is .................: ${WITH_MP}")
#message(STATUS "with factory ...............: ${WITH_FACTORY}")
#message(STATUS "with libfac ................: ${WITH_LIBFAC}")
#message(STATUS "with apint  ................: ${WITH_APINT}")
message(STATUS "with NOSTREAMIO ............: ${WITH_NOSTREAMIO}")


# cpack configuration
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR ${PROJECT_VENDOR})
set(CPACK_PACKAGE_CONTACT "<singular@mathematik.uni-kl.de>")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${V_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${V_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${V_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

# Pack rpms
set(CPACK_RPM_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_RPM_PACKAGE_VENDOR ${PROJECT_VENDOR})
set(CPACK_RPM_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RPM_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_RPM_PACKAGE_VERSION_MAJOR "${V_MAJOR}")
set(CPACK_RPM_PACKAGE_VERSION_MINOR "${V_MINOR}")
set(CPACK_RPM_PACKAGE_VERSION_PATCH "${V_PATCH}")
set(CPACK_RPM_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set(CPACK_SOURCE_RPM ON)
set(CPACK_BINARY_RPM ON)

include(CPack)

