# -*- mode: cmake; -*-
project(Singular)
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)
cmake_policy(VERSION 2.6)
if(POLICY CMP0011)
  cmake_policy(SET CMP0011 NEW)
endif(POLICY CMP0011)

# have the full monty in makefiles
set(CMAKE_VERBOSE_MAKEFILE true)

set(SINGULAR_MAJOR_VERSION 3)
set(SINGULAR_MINOR_VERSION 1)
set(SINGULAR_SUB_VERSION   0)
set(VERSION_SEP "-")
set(SINGULAR_SHORT_VERSION "${SINGULAR_MAJOR_VERSION}${VERSION_SEP}${SINGULAR_MINOR_VERSION}")
set(SINGULAR_VERSION "${SINGULAR_SHORT_VERSION}${VERSION_SEP}${SINGULAR_SUB_VERSION}")
set(SINGULAR_RPM_VERSION "${SINGULAR_MAJOR_VERSION}.${SINGULAR_MINOR_VERSION}.${SINGULAR_SUB_VERSION}")
set(VERSION_DATE "2010-01-12")

###########################################################
# add a path where some libraries might be stored
set(CMAKE_ADDITIONAL_PATH "$ENV{CMAKE_ADDITIONAL_PATH}" 
    CACHE PATH "Path where many locally installed libraries can be found")

###########################################################
# Where are the additional libraries installed? Note: provide includes
# path here, subsequent checks will resolve everything else
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${CMAKE_ADDITIONAL_PATH}/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CMAKE_ADDITIONAL_PATH}/lib)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")


include(CTest)
enable_testing()
add_subdirectory(tests)
