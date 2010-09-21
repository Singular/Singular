# -*- mode: cmake; -*-

include(SingularMacros)

file(GLOB lib_srcs *.lib)
 
singular_copy_LIB(${lib_srcs})

# currently we do NOT create the file all.lib.
# for now it is part of the svn-repository
# singular_mk_allLIB(gen-lib "all.lib" ${lib_srcs})
