# LLVM Includes
include_directories(${LLVM_INCLUDE_DIR})
# Generated Clang includes
include_directories(${CLANG_INCLUDE_DIR})
# Standard Clang includes
include_directories("${CMAKE_SOURCE_DIR}/../clang/include")

set(LLVM_LINK_COMPONENTS FrontEndOpenMP Support)

add_vara_executable(
  hidden-variability-finder HiddenVariabilityFinderDriver.cpp
  HiddenVariabilityMatcherCallbacks.cpp
)

target_link_libraries(
  hidden-variability-finder
  PRIVATE clangAST
          clangASTMatchers
          clangBasic
          clangFrontend
          clangSerialization
          clangTooling
          clangToolingRefactoring
)

install(TARGETS hidden-variability-finder RUNTIME DESTINATION bin)
