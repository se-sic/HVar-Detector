# LLVM Includes
include_directories(${LLVM_INCLUDE_DIR})
# Generated Clang includes
include_directories(${CLANG_INCLUDE_DIR})
# Standard Clang includes
include_directories("${CMAKE_SOURCE_DIR}/../clang/include")

include_directories(include)

add_subdirectory(src)
