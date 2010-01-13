# -*- mode: cmake; -*-
#

## add the CPPUNIT stuff
include_directories(${CPPUNIT_INCLUDE_DIRS})
link_directories(${CPPUNIT_LIBRARY_DIRS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_SOURCE_DIR}/kernel)
include_directories(${${PROJECT_NAME}_SOURCE_DIR}/Singular)
include_directories(${${PROJECT_NAME}_SOURCE_DIR}/omalloc)
include_directories(${${PROJECT_NAME}_SOURCE_DIR}/factory)

link_directories(${${PROJECT_NAME}_SOURCE_DIR}/x86_64-Linux/lib/)
link_directories(${${PROJECT_NAME}_SOURCE_DIR}/kernel)
link_directories(${${PROJECT_NAME}_SOURCE_DIR}/Singular)

file(GLOB test_srcs "test_*.cpp" )
file(GLOB test_hdrs "*.hpp")

add_library(Singular-test ${test_hdrs} ${test_srcs} utils.cpp)
target_link_libraries(Singular-test
  cppunit
  singular.a
  ${${PROJECT_NAME}_SOURCE_DIR}/Singular/libsingular-tesths.o
  gmp readline
  ${CPPUNIT_STATIC_LIBRARIES}
  )

add_executable(Singular_test_suite ${CMAKE_CURRENT_SOURCE_DIR}/../main.cpp)
target_link_libraries(Singular_test_suite Singular-test)

# adding the tests
add_test(Singular_test_suite ${CMAKE_CURRENT_BINARY_DIR}/Singular_test_suite)
