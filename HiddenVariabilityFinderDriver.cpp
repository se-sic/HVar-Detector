// Declares clang::SyntaxOnlyAction.

#include "HiddenVariabilityMatcherCallbacks.h"
#include "HiddenVariabilityMatchers.h"

#include <fstream>

using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace llvm;
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
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

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
  VarDeclMatcherCallback<vara::CandidateType::GlobalConstVar>
      GlobalConstVarCallback(*Executor->get()->getExecutionContext(), Results,
                             RootDirectory);

  VarDeclMatcherCallback<vara::CandidateType::LocalConstVar>
      LocalConstVarCallback(*Executor->get()->getExecutionContext(), Results,
                            RootDirectory);

  FieldDeclMatcherCallback ConstFieldMatcherCallback(
      *Executor->get()->getExecutionContext(), Results, RootDirectory);

  TemplateLiteralMatcherCallback TemplateLiteralMatcherCB(
      *Executor->get()->getExecutionContext(), Results, RootDirectory);

  Finder.addMatcher(vara::GlobalConstVarMatcher.bind(
                        vara::toString(vara::CandidateType::GlobalConstVar)),
                    &GlobalConstVarCallback);

  Finder.addMatcher(vara::LocalConstVarMatcher.bind(
                        vara::toString(vara::CandidateType::LocalConstVar)),
                    &LocalConstVarCallback);

  Finder.addMatcher(vara::ConstFieldMatcher.bind(
                        vara::toString(vara::CandidateType::ConstField)),
                    &ConstFieldMatcherCallback);

  Finder.addMatcher(vara::TemplateLiteralMatcher.bind(
                        vara::toString(vara::CandidateType::TemplateLiteral)),
                    &TemplateLiteralMatcherCB);

  std::filesystem::path OutputPath = std::string(OutFile);

  if (OutputPath.filename().empty()) {
    llvm::outs() << "No proper file name. Using standard output file name.\n";
    OutputPath.replace_filename(OutFile.Default.getValue());
  }

  if (OutputPath.extension() != ".yml") {
    llvm::outs()
        << "Report file should be a '.yml' file! Extension will be changed.\n";
    OutputPath.replace_extension(".yml");
  }

  auto Err = Executor->get()->execute(newFrontendActionFactory(&Finder));
  if (Err) {
    llvm::errs() << llvm::toString(std::move(Err)) << "\n";
  }

  std::error_code EC;
  raw_fd_ostream Ostream(OutputPath.native(), EC);

  if (EC) {
    llvm::errs() << "Error while opening report file!\n";
    return 1;
  }

  llvm::yaml::Output YamlOutput(Ostream);

  YamlOutput << Results;
}
