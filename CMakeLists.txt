cmake_minimum_required(VERSION 3.1)
project(variflect)

find_package(Clang REQUIRED)
message(STATUS "CLANG LIBRARIES: ${CLANG_LIBRARIES} ${CLANG_INCLUDE_DIRS}")

include_directories(include ${CLANG_INCLUDE_DIRS})

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

add_executable(variflect
  include/variflect.hh
  src/variflect.cc
)

target_link_libraries(variflect
  clang
)

target_include_directories(variflect PUBLIC
  thirdparty/json/include
)