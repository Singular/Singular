# -*- mode: cmake; -*-
#
#

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})

#include_directories(${CMAKE_SOURCE_DIR}/factory)
include_directories(${CMAKE_SOURCE_DIR}/ntl/include)

add_subdirectory(LIB)
include(SingularMacros)

singular_mk_version(SINGULAR_VERSION_H)

message(STATUS ">>DIST: '${MAKE_DISTRIBUTION}'")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/distrib.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/distrib.h )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
#################################################################
###  check for lex/flex and add parser files
### 
find_program(FLEX flex)

include(LexMacros)
check_lex_version(GEN_NEW_LEX)
add_lex(SCANNER_CPP ${CMAKE_CURRENT_SOURCE_DIR}/scanner.l "")
add_lex(LIBPARSE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/libparse.l "yylp")
message(STATUS "Add lex ${SCANNER_CPP}")
message(STATUS "Add lex ${LIBPARSE_CPP}")

#################################################################
###  check for bison/yacc and grammar files
### 
include(BisonMacros)
add_grammar(GRAMMAR_TARGET GRAMMAR_CPP GRAMMAR_H ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y)

#################################################################
###
###
set(_feopt_srcs feOpt.cc mod2.h)
add_executable(feOpt ${_feopt_srcs})
set_target_properties(feOpt PROPERTIES
  COMPILE_DEFINITIONS "${ADD_DEFS};GENERATE_OPTION_INDEX")
add_dependencies(feOpt omalloc)
#singular_generate_incfile(FEOPT_TARGET FEOPT_FILE_INC
singular_generate_incfile(feopt_target FEOPT_FILE_INC
  ${CMAKE_CURRENT_BINARY_DIR}/feOpt feOpt)
#  add_custom_target(feopt_target DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/feOpt
#    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/feOpt
#    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/feOpt
#    COMMENT "Creating 'feopt.inc'....."
#    )
#add_index(FEOPT_FILE_INC ${CMAKE_CURRENT_BINARY_DIR}/feOpt feOpt)

#################################################################
###
###
add_executable(feOptES ${_feopt_srcs})
set_target_properties(feOptES PROPERTIES COMPILE_DEFINITIONS "${ADD_DEFS};GENERATE_OPTION_INDEX -DESINGULAR")
add_dependencies(feOptES omalloc)
singular_generate_incfile(feOptES_target FEOPTES_FILE_INC
  ${CMAKE_CURRENT_BINARY_DIR}/feOptES feOptES)
#add_index(FEOPTES_FILE_INC ${CMAKE_CURRENT_BINARY_DIR}/feOptES feOptES)

#################################################################
###
###
add_executable(feOptTS ${_feopt_srcs})
set_target_properties(feOptTS PROPERTIES COMPILE_DEFINITIONS "${ADD_DEFS};GENERATE_OPTION_INDEX -DTSINGULAR")
add_dependencies(feOptTS omalloc)
singular_generate_incfile(feoptsts_target FEOPTSTS_FILE_INC
  ${CMAKE_CURRENT_BINARY_DIR}/feOptTS feOptTS)
#add_index(FEOPTSTS_FILE_INC ${CMAKE_CURRENT_BINARY_DIR}/feOptTS feOptTS)

#################################################################
###  build gentable and create iparith.inc
###

# C++ sources which are used if no parts of the kernel are dynamically linked
set(static_sources slInit_Static.cc)

set(_iparith_inc_srcs  iparith.cc tesths.cc tok.h mpsr_Tok.cc mod2.h)
add_executable(gentable ${_iparith_inc_srcs} claptmpl.cc ${static_sources})
add_dependencies(gentable omalloc)
set_target_properties(gentable PROPERTIES
  COMPILE_DEFINITIONS "${ADD_DEFS};GENTABLE=1")

#target_link_libraries(gentable omalloc_ndebug sing-main kernel gmp fac singcf sing-gentable ${NTL_LIBRARY} )
#LIBS           = -lreadline -lhtmlhelp -Xlinker -Bdynamic -lgmp -lomalloc -lncurses

#target_link_libraries(gentable sing-main omalloc_ndebug kernel_mmalloc)
target_link_libraries(gentable sing-main)
target_link_libraries(gentable kernel)
target_link_libraries(gentable singfac)
target_link_libraries(gentable singcf)
target_link_libraries(gentable ${NTL_LIBRARY})
target_link_libraries(gentable omalloc  )

target_link_libraries(gentable gmp)
target_link_libraries(gentable readline)
target_link_libraries(gentable ncurses)
target_link_libraries(gentable m)

singular_generate_incfile(iparith_target IPARITH_INC
  ${CMAKE_CURRENT_BINARY_DIR}/gentable iparith)
#singular_gen_iparith(IPARITH_TARGET IPARITH_INC ${CMAKE_CURRENT_BINARY_DIR}/gentable
#  ${_iparith_inc_srcs})
message(STATUS "====== '${IPARITH_INC}' ====")

##########
#
# normal C++ source files
set(cxxsources ${GRAMMAR_CPP}  ${SCANNER_CPP} attrib.cc
  ${LIBPARSE_CPP}
  ${SINGULAR_VERSION_H}
  ${FEOPT_FILE_INC}
    eigenval_ip.cc
    extra.cc fehelp.cc feOpt.cc
    ipassign.cc ipconv.cc ipid.cc iplib.cc
    ipprint.cc ipshell.cc
    lists.cc
    sdb.cc
    fglm.cc interpolation.cc
    silink.cc ssiLink.cc
    subexpr.cc
    janet.cc wrapper.cc
    sing_win.cc
    gms.cc pcv.cc maps_ip.cc
    walk.cc walk_ip.cc
    cntrlc.cc misc_ip.cc
    calcSVD.cc pipeLink.cc
    Minor.cc
    MinorProcessor.cc
    MinorInterface.cc
)

# C++ sources which are used if part of the kernel are dynamically linked
set(dl_kernel_sources slInit_Dynamic.cc )

# C++ sources which are used for dynamic libraries if part of the kernel is
# dynamically linked
set(dl_lib_sources "")

# stuff for dbm
set(dbmsr_sources ndbm.cc sing_dbm.cc)

###
if( ${GEN_NEW_LEX} EQUAL 1)
  set(FLEXFLAGS ";NEW_FLEX")
else( ${GEN_NEW_LEX} EQUAL 1)
  set(FLEXFLAGS "")
endif( ${GEN_NEW_LEX} EQUAL 1)
add_library(sing-main ${cxxsources} ${dbmsr_sources})
set_target_properties(sing-main PROPERTIES
  COMPILE_DEFINITIONS "${ADD_DEFS}${FLEXFLAGS}")
set_target_properties(sing-main PROPERTIES
  COMPILE_FLAGS "-fno-implicit-templates")

#message(STATUS "=== '${CMAKE_CXX_FLAGS_DEBUG}'")
#add_library(sing-main_g ${cxxsources})
#set_target_properties(sing-main_g PROPERTIES COMPILE_FLAGS
#  "-fno-implicit-templates${CMAKE_CXX_FLAGS_DEBUG}")
#
message("Grammar creates ${GRAMMAR_CPP}...")
message("Grammar creates ${GRAMMAR_H}...")

# stuff for MP
set(mpsr_sources mpsr_Put.cc mpsr_PutPoly.cc mpsr_GetPoly.cc mpsr_sl.cc
        mpsr_Get.cc mpsr_GetMisc.cc mpsr_Error.cc )


# normal C source files
set(CSOURCES "")

# C++ sources for which both, shared and static object files are needed
set(common_sources ${mpsr_sources} ${dbmsr_sources} )

# special C++ source files (need extra compiling and/or linking), for which
# dependencies should be generated
set(esources ${IPARITH_INC} iparith.cc ${common_sources}
  ${dl_kernel_sources} ${dl_lib_sources} ${static_sources} ) #
set_source_files_properties(iparith.cc PROPERTIES OBJECT_DEPENDS ${IPARITH_INC})

set(sources ${csources} ${cxxsources}
        grammar.y scanner.l libparse.l
        utils.cc utils.h
        tesths.cc mpsr_Tok.cc claptmpl.cc )

set(headers lists.h attrib.h
        mpsr_Tok.h
        ipconv.h
        subexpr.h ipid.h
        cntrlc.h ipprint.h sdb.h
        ipshell.h tok.h
        silink.h
        sing_dbm.h maps_ip.h interpolation.h
        mpsr.h mpsr_sl.h
        mpsr_Get.h janet.h
        mpsr_Put.h walk.h
        dbm_sl.h libparse.h
        gms.h pcv.h eigenval_ip.h
        walk.h
        static.h
        omSingularConfig.h)

add_executable(Singular-static
  tesths.cc iparith.cc mpsr_Tok.cc claptmpl.cc ${static_sources})
set_target_properties(Singular-static PROPERTIES
  COMPILE_DEFINITIONS "${ADD_DEFS}")

set_target_properties(Singular-static PROPERTIES COMPILE_DEFINIIONS "${ADD_DEFS}")
target_link_libraries(Singular-static sing-main)
target_link_libraries(Singular-static kernel)
target_link_libraries(Singular-static singfac)
target_link_libraries(Singular-static singcf)
target_link_libraries(Singular-static ${NTL_LIBRARY})
target_link_libraries(Singular-static omalloc)

target_link_libraries(Singular-static gmp)
target_link_libraries(Singular-static readline)
target_link_libraries(Singular-static tinfo)

#add_dependencies(Singular-static ${OMALLOC_TARGET})
add_dependencies(Singular-static gen-omalloc-h)
add_dependencies(Singular-static omalloc)
### add_dependencies(Singular-static iparith_target)
install(TARGETS Singular-static DESTINATION bin)

# build libparse
set(libparse_srcs ${LIBPARSE_CPP} utils.cc)
add_executable(libparse ${libparse_srcs})
set_target_properties(libparse PROPERTIES
  COMPILE_DEFINITIONS "${ADD_DEFS};STANDALONE_PARSER${FLEXFLAGS}")
target_link_libraries(libparse sg-fegetopt)
target_link_libraries(libparse omalloc_ndebug)
add_dependencies(libparse omalloc)
install(TARGETS libparse DESTINATION bin)

file(GLOB LIB_srcs LIB/*lib)
install(FILES ${LIB_srcs} DESTINATION lib/LIB)

#set(libsingular_srcs ${SCANNER_CPP} ${LIBPARSE_CPP} ${IPARITH_INC}  ${static_sources}
set(libsingular_srcs iparith.cc tesths.cc  claptmpl.cc mpsr_Tok.cc )

# mod2.h SINGULAR_VERSION_H
#target_link_libraries(singular sing-main kernel singfac singcf ntl readline gmp omalloc htmlhelp)

install(FILES libsingular.h DESTINATION include)

##merge_libraries(singfac singcf ntl readline gmp omalloc)
add_library(singular_ndebug ${libsingular_srcs})
set_target_properties(singular_ndebug PROPERTIES
  COMPILE_DEFINITIONS "${ADD_DEFS};LIBSINGULAR")
##merge_libraries(SINGULARLIB_TARGET GEN_LIB_A singular_ndebug kernel singfac singcf omalloc_ndebug)
##message(STATUS "Merge-Target: '${SINGULARLIB_TARGET}'")
##add_dependencies(singular_ndebug ${SINGULARLIB_TARGET})
##add_dependencies(${SINGULARLIB_TARGET} ${OMALLOC_TARGET} ${IPARITH_TARGET})
##add_dependencies(${SINGULARLIB_TARGET} singular_main kernel singfac singcf  omalloc_ndebug)

install(TARGETS ${GEN_TARGET} DESTINATION lib)
#libparse.cc scanner.cc


#add_library(Singularg STATIC
#  ${IPARITH_INC}
#  tesths.cc iparith.cc mpsr_Tok.cc claptmpl.cc)
#set_target_properties(Singularg PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
#target_link_libraries(Singular-static sing-main kernel_g gmp fac singcf ntl omalloc)

#add_library(Singularp STATIC ${IPARITH_INC}
#  tesths.cc iparith.cc mpsr_Tok.cc claptmpl.cc)
#set_target_properties(Singularp PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS_PROFILE})
#target_link_libraries(Singular-static sing-main kernel_p gmp fac singcf ntl omalloc_p)

#add_dependencies(extra.o feopt_target)
#set_source_files_properties(extra.cc PROPERTIES OBJECT_DEPENDS ${FEOPT_FILE_INC})
set_source_files_properties(${FEOPT_FILE_INC} PROPERTIES GENERATED true)
#set_source_files_properties(feopt.h PROPERTIES OBJECT_DEPENDS feopt_target)
#cntrlc.cc
#extra.cc
#fehelp.cc
#feOpt.cc
#mpsr_sl.cc
#tesths.cc
