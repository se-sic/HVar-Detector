if((NOT DEFINED VARA_SOURCE_DIR) OR (NOT DEFINED LLVM_MAIN_SRC_DIR))
  # Support out of tree builds independent of VARA and LLVM Loosely based on
  # https://github.com/firolino/clang-tool/
  cmake_minimum_required(VERSION 3.13)
  project(hidden-variability-finder)

  # Set C++ standard
  set(CMAKE_CXX_STANDARD 20)

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

  # Custom add executable macro
  macro(CUSTOM_ADD_EXECUTABLE name)
    add_executable(${name} ${ARGN})
  endmacro()

  find_package(LLVM REQUIRED CONFIG)
  find_package(Clang REQUIRED CONFIG)
else()
  include_directories("${CMAKE_SOURCE_DIR}/../clang/include")
  # Custom add executable macro
  macro(CUSTOM_ADD_EXECUTABLE name)
    add_vara_executable(${name} ${ARGN})
  endmacro()

  set(IN_VARA_BUILD TRUE)
endif()

set(HVARDETECTOR_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(HVARDETECTOR_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# LLVM Includes
include_directories(${LLVM_INCLUDE_DIR})
# Generated Clang includes
include_directories(${CLANG_INCLUDE_DIR})
# Standard Clang includes
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
# Add LLVM's cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_INSTALL_ROOT}")
include(AddLLVM)

# Debug: Print LLVM and Clang include dirs
message(STATUS "LLVM includes: ${LLVM_INCLUDE_DIR}")
message(STATUS "Clang includes: ${CLANG_INCLUDE_DIR}")
message(STATUS "LLVM libs: ${LLVM_LIBRARY_DIR}")
message(STATUS "LLVM Definitions: ${LLVM_DEFINITIONS}")

include_directories(include)

add_subdirectory(src)
add_subdirectory(tests)
