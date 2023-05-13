cmake_minimum_required(VERSION 3.1)
project (variflect)

include_directories(include)

set (CMAKE_BUILD_TYPE Debug)

set (CMAKE_CXX_STANDARD 20)

add_library(variflect
  include/variant.hh
  src/variant.cc

  include/json_parser.hh
  src/json_parser.cc
)

target_include_directories(variflect PUBLIC
  thirdparty/json/include
)

add_executable(test_variant
  test/test_variant.cc
)

target_link_libraries(test_variant variflect)