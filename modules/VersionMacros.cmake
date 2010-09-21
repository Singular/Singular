# -*- mode: cmake; -*-
#
# create file kversion.h
#
function(create_version GEN_VERSION_HEADER)
  set(${GEN_VERSION_HEADER} ${CMAKE_CURRENT_BINARY_DIR}/kversion.h)
  execute_process(COMMAND date +%Y%m%d%H
    OUTPUT_VARIABLE  _version)
  file(WRITE ${${GEN_VERSION_HEADER}} "#define feVersionId0 ${_version}" )
endfunction()
