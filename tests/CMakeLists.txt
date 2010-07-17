# -*- mode: cmake; -*-
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_SOURCE_DIR}/kernel)
include_directories(${${PROJECT_NAME}_SOURCE_DIR}/factory)

# include directories should be solved
#include_directories(${${PROJECT_NAME}_SOURCE_DIR}/Singular)
include_directories(${${PROJECT_NAME}_SOURCE_DIR}/omalloc)
include_directories(${${PROJECT_NAME}_SOURCE_DIR}/x86_64-Linux/include )


add_subdirectory(Singular)
add_subdirectory(kernel)

set(test_srcs main.cpp
#	Singleton.cpp
  )

add_executable(Singular_test_suite ${test_srcs})
target_link_libraries(Singular_test_suite Singular-test)
target_link_libraries(Singular_test_suite ${${PROJECT_NAME}_SOURCE_DIR}/omalloc/libomalloc.a)

#target_link_libraries(Singular_test_suite Singular-test)

# adding the tests
add_test(Singular_test_suite ${CMAKE_CURRENT_BINARY_DIR}/Singular_test_suite)

