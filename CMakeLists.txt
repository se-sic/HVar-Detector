if((NOT DEFINED VARA_SOURCE_DIR) OR (NOT DEFINED LLVM_MAIN_SRC_DIR))
  # Support out of tree builds independent of VARA and LLVM Loosely based on
  # https://github.com/firolino/clang-tool/
  project(hidden-variability-finder)
  cmake_minimum_required(VERSION 3.13)

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

  execute_process(
    COMMAND bash -c
            "$VARATS_ROOT/tools/VaRA/bin/clang++ -E -x c++ - -v < /dev/null 2>&1
      | grep \"lib/clang\" | awk '{{print $1}}' | grep include"
    OUTPUT_VARIABLE clangBuiltInIncludePath
  )

  file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/builtInInclude.path"
       "${clangBuiltInIncludePath}"
  )

  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

  # Custom add executable macro
  macro(CUSTOM_ADD_EXECUTABLE name)
    add_executable(${name} ${ARGN})
  endmacro()

  find_package(LLVM REQUIRED CONFIG)
  find_package(
    Clang
    REQUIRED
    CONFIG
    HINTS
    ${CLANG_INSTALL_ROOT}
  )
else()
  include_directories("${CMAKE_SOURCE_DIR}/../clang/include")
  # Custom add executable macro
  macro(CUSTOM_ADD_EXECUTABLE name)
    add_vara_executable(${name} ${ARGN})
  endmacro()
endif()

# LLVM Includes
include_directories(${LLVM_INCLUDE_DIR})
# Generated Clang includes
include_directories(${CLANG_INCLUDE_DIR})
# Standard Clang includes

# Debug: Print LLVM and Clang include dirs
message(STATUS "LLVM includes: ${LLVM_INCLUDE_DIR}")
message(STATUS "Clang includes: ${CLANG_INCLUDE_DIR}")
message(STATUS "LLVM libs: ${LLVM_LIBRARY_DIR}")
message(STATUS "LLVM Definitions: ${LLVM_DEFINITIONS}")

include_directories(include)

add_subdirectory(src)
