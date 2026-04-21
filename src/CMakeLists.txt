set(LLVM_LINK_COMPONENTS FrontEndOpenMP Support)

custom_add_executable(
  hidden-variability-finder
  HiddenVariabilityFinderDriver.cpp
  Matchers/ConstVarMatcher.cpp
  Matchers/ConstFieldMatcher.cpp
  Matchers/TemplateLiteralMatcher.cpp
  Matchers/LiteralBinOpMatcher.cpp
)

target_link_libraries(
  hidden-variability-finder
  PRIVATE ${CLANG_LIBS}
          ${LLVM_LIBS_CORE}
          ${LLVM_LDFLAGS}
          clangAST
          clangASTMatchers
          clangBasic
          clangFrontend
          clangSerialization
          clangTooling
          clangToolingRefactoring
)
