# -*- mode: cmake; -*-
cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)
if(POLICY CMP0011)
  cmake_policy(SET CMP0011 NEW)
endif(POLICY CMP0011)

message(STATUS "Header fac: '${OMALLOC_H}'")
project(factory)
string(TOUPPER ${PROJECT_NAME} UPPER_PROJECT_NAME)

# have the full monty in makefiles
set(CMAKE_VERBOSE_MAKEFILE true)

set(FACTORY_VERSION_MAJOR 3)
set(FACTORY_VERSION_MINOR 1)
set(FACTORY_VERSION_PATCH 0)
set(FACTORYVERSION "${FACTORY_VERSION_MAJOR}.${FACTORY_VERSION_MINOR}.${FACTORY_VERSION_PATCH}")
set(FACTORYCONFIGURATION "xxx")
set(GFTABLEDIR "share/factory/gftables")

OPTION(WITH_OMALLOC   "omalloc memory managment" ON)
OPTION(WITH_GMP       "specify where to find gmp library" On)
##OPTION(WITH_SINGULAR  "build for use with Singular" Off)
OPTION(WITH_NTL       "build for use with NTL." Off)
##OPTION(WITH_SINGULARERROR "use error reporting from Singular" Off)
OPTION(WITH_NOSTREAMIO    "do not require iostream include nor library" Off)
OPTION(WITH_STREAMIO    "build Factory without stream IO" Off)

OPTION(ENABLE_CF_INLINE "build Factory with \"configurable inline methods\" enabled." Off)
OPTION(ENABLE_MEMDEBUG  "switch on memory management debugging.  Meaningful
                          with --DWITH_MEMMAN=new' only.  <level> may be either
                          no (default), normal, or full" Off)
OPTION(ENABLE_ASSERTIONS "build Factory with assertions activated" Off)
OPTION(ENABLE_TIMING     "build Factory so it will print timing information" Off)
OPTION(ENABLE_DEBUGOUTPUT "build Factory so it will print debugging information" Off)
OPTION(ENABLE_GMP 
  "together with --DWITH_SINGULAR means: installation in process, be graceful when there is no gmp.h" Off)

set(HAVE_NTL 1)
set(NOASSERT 1)
set(SINGULAR 1)
set(HAVE_OMALLOC 1)
#set(NOSTREAMIO 1)

include(BisonMacros)
set(OMALLOC_TARGET gen-omalloc-h)
#set(OMALLOC_TARGET omalloc)
#set(ADD_DEFS "NOSTREAMIO;${_OSDEF};HAVE_CONFIG_H;SINGULAR")
set(ADD_DEFS "${_OSDEF};HAVE_CONFIG_H;SINGULAR")

###################################################################################################
# No user settings below
###################################################################################################
check_headers(stdio.h stdlib.h string.h time.h math.h cstdio)
if(ENABLE_STREAMIO)
  check_headers(iostream.h strstream.h fstream.h iostream string fstream)
endif(ENABLE_STREAMIO)
check_headers(ctype.h)
check_headers(stdarg.h signal.h errno.h unistd.h)
# endif(ENABLE_STREAMIO)

# if(ENABLE_TIMING)
check_headers(sys/param.h sys/times.h)
# endif(ENABLE_TIMING)

check_headers(omalloc.h)
message(STATUS "=== ${HAVE_STRING_H} ==")

include(FactoryMacros)
###################################################################################################
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  message(STATUS "Have compiler: '${CMAKE_CXX_COMPILER_ID}'")
  if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")

    #set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -Wno-non-virtual-dtor")
    set(CMAKE_CXX_FLAGS "-w --no-rtti -pipe ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-implicit-templates")
    #add_definitions(-DNDEBUG -DOM_NDEBUG -Dx86_64_Linux -DHAVE_CONFIG_H)

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
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)

factory_header(FACTORYCONF_H factoryconf.template)
factory_header(FACTORY_H factory.template)
factory_gen_gmp_header(FACTORY_TARGET CF_GMP_H)
set(FACTORY_TARGET ${FACTORY_TARGET} PARENT_SCOPE)

#add_definitions(-DNDEBUG -DOM_NDEBUG -Dx86_64_Linux -DHAVE_CONFIG_H)
# factory source files
set_source_files_properties(${OMALLOC_H} PROPERTIES GENERATED true )
set(basefactory_srcs
  ${OMALLOC_H}
  ${FACTORYCONF_H} ${FACTORY_H} ${CF_GMP_H}
  canonicalform.cc 
  cf_algorithm.cc 
  cf_binom.cc 
  cf_char.cc 
  cf_chinese.cc 
  cf_cyclo.cc
  cf_eval.cc 
  cf_factor.cc 
  cf_factory.cc 
  cf_gcd.cc 
  cf_gcd_charp.cc 
  cf_gcd_smallp.cc 
  cf_generator.cc 
  cf_globals.cc 
  cf_hnf.cc
  cf_inline.cc 
  cf_irred.cc 
  cf_iter.cc 
  cf_iter_inline.cc 
  cf_linsys.cc 
  cf_map.cc 
  cf_map_ext.cc 
  cf_ops.cc 
  cf_primes.cc 
  cf_random.cc 
  cf_resultant.cc 
  cf_reval.cc 
  cf_switches.cc 
  cf_util.cc 
  debug.cc 
  DegreePattern.cc
  ExtensionInfo.cc
  fac_berlekamp.cc 
  fac_cantzass.cc 
  fac_distrib.cc 
  fac_ezgcd.cc 
  fac_iterfor.cc 
  fac_multihensel.cc 
  fac_multivar.cc 
  fac_sqrfree.cc 
  fac_univar.cc 
  fac_util.cc 
  facFqBivar.cc
  facFqBivarUtil.cc
  facFqFactorize.cc
  facFqFactorizeUtil.cc
  facFqSquarefree.cc
  facHensel.cc
  fieldGCD.cc 
  ffops.cc 
  ffreval.cc 
  gf_tabutil.cc 
  gfops.cc 
  imm.cc 
  initgmp.cc 
  int_cf.cc 
  int_int.cc 
  int_intdiv.cc 
  int_poly.cc 
  int_pp.cc 
  int_rat.cc 
  sm_sparsemod.cc 
  sm_util.cc 
  variable.cc 
  NTLconvert.cc 
  abs_fac.cc 
  bifac.cc 
  lgs.cc
)
set(singfactory_srcs singext.cc)
#set_source_files_properties(${basefactory_srcs} PROPERTIES OBJECT_DEPENDS 
#  ${FACTORYCONF_H} ${FACTORY_H} ${CF_GMP_H})
set(FACTORYCONF_H ${FACTORYCONF_H} PARENT_SCOPE)

add_grammar(READCF_TARGET READCF_CPP READCF_H ${CMAKE_CURRENT_SOURCE_DIR}/readcf.y)
set(useiofactory_src parseutil.cc ${READCF_CPP})
set(distfactory_src ${basefactory_srcs} ${singfactory_srcs} ${useiofactorysrc})

set(doxy_src ${basefactory_srcs} parseutil.cc)

# factory header files
set(basefactory_hdrs
                assert.h
                canonicalform.h 
                cf_algorithm.h 
                cf_binom.h 
                cf_defs.h 
                cf_eval.h 
                cf_factory.h 
                cf_generator.h 
                cf_globals.h 
                cf_irred.h 
                cf_iter.h 
                cf_map.h 
                cf_primes.h 
                cf_primetab.h 
                cf_random.h 
                cf_reval.h 
                cf_switches.h 
                cf_util.h 
                debug.h 
                fac_berlekamp.h 
                fac_cantzass.h 
                fac_distrib.h 
                fac_iterfor.h 
                fac_multivar.h 
                fac_sqrfree.h 
                fac_univar.h 
                fac_util.h 
                fieldGCD.h 
                ffops.h 
                ffreval.h 
                ftmpl_array.h 
                ftmpl_factor.h 
                ftmpl_functions.h 
                ftmpl_list.h 
                ftmpl_matrix.h 
                gf_tabutil.h 
                gfops.h 
                gmpext.h 
                imm.h 
                int_cf.h 
                int_int.h 
                int_poly.h 
                int_pp.h 
                int_rat.h 
                sm_sparsemod.h 
                sm_util.h 
                timing.h 
                variable.h 
                NTLconvert.h 
                bifac.h 
                bifacConfig.h 
                lgs.h 
                algext.h
)

#SET_SOURCE_FILES_PROPERTIES(${basefactory_srcs} 
#  OBJECT_DEPENDS ../omalloc/omalloc.h})

set(singfactory_hdrs singext.h)
set(useiofactory_hdrs parseutil.h)
set(distfactory_hdrs ${basefactory_hdrs} ${singfactory_hdrs} ${useiofactory_hdrs})
set(doxy_hdrs ${basefactory_hdrs} parseutil.h)

# memory manager source files
set(basememman_src newdelete.cc)
set(newmemman_src 
                mmalloc.c 
                mmallocb.c 
                mmallocs.c 
                mmblock.c 
                mmheap.c 
                mminit.c 
                mmspec.c 
                mmutil.c
)
set(oldmemman_src memutil.c)
set(distmemman_src ${basememmansrc} ${newmemmansrc} ${oldmemmansrc})

# memory manager header files
set(newmemman_hdrs mmprivate.h memman.h)
set(oldmemman_hdrs memutil.h)
set(distmemman_hdrs ${newmemman_hdrs} ${oldmemman_hdrs})

# factory template files
set(template_src
  ftmpl_array.cc 
  ftmpl_factor.cc 
  ftmpl_functions.h 
  ftmpl_list.cc 
  ftmpl_matrix.cc
)
set_source_files_properties(${template_srcs} PROPERTIES OBJECT_DEPENDS 
  ${FACTORYCONF_H} ${FACTORY_H} ${CF_GMP_H})
set(template_hdrs
                ftmpl_array.h 
                ftmpl_factor.h 
                ftmpl_list.h 
                ftmpl_matrix.h
)

# header templates
set(hdrtempl_src factoryconf.template  factory.template)
##set(hdrtempl_dep $(hdrtemplsrc:.template=.d)

# FTE source files
set(ftest_src   ftest_io.cc ftest_util.cc ntl_util.cc)

# FTE header files
set(ftest_hdrs  ftest_io.h ftest_util.h ntl_util.h)

# define libraries
#add_library(cf ${FACTORYCONF_H} ${basefactory_srcs})
add_library(cf ${basefactory_srcs} ${singfactory_srcs})
set_target_properties(cf PROPERTIES COMPILE_DEFINITIONS "${ADD_DEFS}")
add_dependencies(cf ${OMALLOC_TARGET})
add_dependencies(cf ${NTL_TARGET})

#
# libraries for Singular
if( WITH_SINGULAR )
  # define libraries
  message(STATUS "Compiling factory for Singular: creating libsingcf.a")
  add_library(singcf ${basefactory_srcs} ${singfactory_srcs})
  set_target_properties(singcf PROPERTIES COMPILE_DEFINITIONS "${ADD_DEFS}")
  add_dependencies(singcf ${OMALLOC_TARGET} ${NTL_TARGET})
else( WITH_SINGULAR )
  message(STATUS "Disabled creation of libsingcf.a")
endif( WITH_SINGULAR )


add_library(mem-old ${oldmemman_src})
set_target_properties(mem-old PROPERTIES COMPILE_DEFINITIONS "${ADD_DEFS}")

add_library(mem-new ${newmemman_src})
set_target_properties(mem-new PROPERTIES COMPILE_DEFINITIONS "${ADD_DEFS}")
