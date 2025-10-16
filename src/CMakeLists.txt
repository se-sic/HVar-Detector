set(LLVM_LINK_COMPONENTS FrontEndOpenMP Support)

custom_add_executable(
  hidden-variability-finder HiddenVariabilityFinderDriver.cpp
  MatcherCallbacks.cpp
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
