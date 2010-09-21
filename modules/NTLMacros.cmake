# -*- mode: cmake; -*-
#
#
#
#    CFLAGS='-O2 -fomit-frame-pointer'
#    CXXFLAGS='-O2 -fomit-frame-pointer -fno-rtti'

function(add_ntl GEN_TARGET GEN_LIB DIR) 
  message(STATUS "add_ntl")
  set(_GEN_LIB "${DIR}/src/ntl.a")
  set(_GEN_TARGET "generate_ntl")
  #    LIBDIR= INCLUDEDIR= PREFIX= 
  set(_cxx_flags "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
  set(_cc_flags "${CMAKE_CC_FLAGS} ${CMAKE_CC_FLAGS_RELEASE}")

  message(STATUS "NTL: Flags and options used to compile ntl")
  message(STATUS "CC=${CMAKE_CC_COMPILER}")
  message(STATUS "CXX=${CMAKE_CXX_COMPILER}")
  message(STATUS "CFLAGS=${_cc_flags}")
  message(STATUS "CXXFLAGS=${_cxx_flags}")
  message(STATUS "NTL_GMP_LIP=on NTL_STD_CXX=on")
  #    CXXFLAGS=\" -O2 -fomit-frame-pointer -fno-rtti\"
  #utility_source(NTL bin path_to_source)
  add_custom_command(
    OUTPUT ${DIR}/src/makefile ${DIR}/include/NTL/config.h ${DIR}/setup4
    COMMAND ./configure CC="gcc" CXX="g++" LDFLAGS="" GMP_PREFIX=/ NTL_GMP_LIP=on  NTL_STD_CXX=on CFLAGS=${_cc_flags} CXXFLAGS=${_cxx_flags}

    COMMAND make

    WORKING_DIRECTORY ${DIR}/src
    DEPENDS ${DIR}/src/makefile
    COMMENT "Compiling 'ntl' number theory library..."
    )

  add_custom_target(generate_ntl DEPENDS 
    ${DIR}/setup4)

  set(${GEN_TARGET} ${_GEN_TARGET} PARENT_SCOPE)
  set(${GEN_LIB}    ${_GEN_LIB} PARENT_SCOPE)

endfunction()
