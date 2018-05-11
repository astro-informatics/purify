cmake_minimum_required (VERSION 2.8)
project (yaml-parser-test)

# C++11
set(CMAKE_CXX_STANDARD 11)


set(src ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(test ${CMAKE_CURRENT_SOURCE_DIR}/test)

message(STATUS "/src found")
message(STATUS "/test found")

find_package(yaml-cpp)
if (NOT yaml-cpp_FOUND)
   message(FATAL_ERROR "yaml-cpp library not found. Please install.")
endif (NOT yaml-cpp_FOUND)

add_compile_options(-Wall)

add_library(yaml-parser-lib src/yaml-parser.cpp)

add_executable(parser-test test/parser-test.cpp)
target_link_libraries(parser-test yaml-parser-lib -lyaml-cpp)
