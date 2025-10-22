// Declares clang::SyntaxOnlyAction.

#include "vara/Matchers/ConstFieldMatcher.h"
#include "vara/Matchers/ConstVarMatcher.h"
#include "vara/Matchers/MatcherBase.h"
#include "vara/Matchers/TemplateLiteralMatcher.h"

#include "llvm/Support/Signals.h"

#include <fstream>

using namespace clang::tooling;
using namespace clang::ast_matchers;
using llvm::yaml::Output;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory HVFOptions("hidden-variability-finder options");

static llvm::cl::opt<std::string>
    OutFile("report-file", llvm::cl::Optional,
            llvm::cl::init("hidden-configurability-report.yml"),
            llvm::cl::desc("File to output results to"),
            llvm::cl::cat(HVFOptions));

static llvm::cl::opt<std::string> RootDir(
    "root-dir", llvm::cl::Optional,
    llvm::cl::desc(
        "Project root directory to use as base for file paths in report file"),
    llvm::cl::cat(HVFOptions));

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static llvm::cl::extrahelp MoreHelp("\nMore help text...\n");

int main(int argc, const char **argv) {
  llvm::outs() << "You are using hidden-variability-finder\n";
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

  vara::ResultMapTy Results;

  auto Executor =
      clang::tooling::createExecutorFromCommandLineArgs(argc, argv, HVFOptions);

  if (!Executor) {
    llvm::errs() << llvm::toString(Executor.takeError()) << "\n";
    return 1;
  }

  MatchFinder Finder;

  std::optional<std::filesystem::path> RootDirectory = std::nullopt;

  if (!RootDir.empty()) {
    auto Path = std::filesystem::path(RootDir.c_str());
    if (std::filesystem::exists(Path)) {
      RootDirectory = std::filesystem::path(RootDir.c_str());
    }
  }

  using namespace vara;
  // TODO: Refactor this with some proper registry such that it can easily be
  // extended with new patterns

  ConstVarMatcher<CandidateType::GlobalConstVar> GCVarMatcher(RootDirectory);
  GCVarMatcher.registerMatcher(Finder);

  ConstVarMatcher<CandidateType::LocalConstVar> LCVarMatcher(RootDirectory);
  LCVarMatcher.registerMatcher(Finder);

  ConstFieldMatcher CFMatcher(RootDirectory);
  CFMatcher.registerMatcher(Finder);

  TemplateLiteralMatcher TLMatcher(RootDirectory);
  TLMatcher.registerMatcher(Finder);

  std::filesystem::path OutputPath = std::string(OutFile);

  if (OutputPath.filename().empty()) {
    llvm::outs() << "No proper file name. Using standard output file name.\n";
    OutputPath.replace_filename(OutFile.Default.getValue());
  }

  if (auto Err = Executor->get()->execute(newFrontendActionFactory(&Finder))) {
    llvm::errs() << llvm::toString(std::move(Err)) << "\n";
  }

  GCVarMatcher.mapResults(Results, GCVarMatcher.getCandidateType());
  LCVarMatcher.mapResults(Results, LCVarMatcher.getCandidateType());
  CFMatcher.mapResults(Results, CFMatcher.getCandidateType());
  TLMatcher.mapResults(Results, TLMatcher.getCandidateType());

  std::error_code EC;
  llvm::raw_fd_ostream Ostream(OutputPath.native(), EC);

  if (EC) {
    llvm::errs() << "Error while opening report file!\n";
    return 1;
  }

  llvm::yaml::Output YamlOutput2(Ostream);
  YamlOutput2 << Results;
}
