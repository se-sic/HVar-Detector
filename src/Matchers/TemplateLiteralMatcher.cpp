//
// Created by abeltluk on 10/22/25.
//
#include "vara/Matchers/TemplateLiteralMatcher.h"
#include "vara/Candidates/TemplateLiteral.h"

namespace vara {

void TemplateLiteralMatcher::registerMatcher(MatchFinder &Finder) {
  auto TemplateMatcher = ast_matchers::TemplateLiteralMatcher.bind(
      vara::toString(this->getCandidateType()));

  Finder.addMatcher(TemplateMatcher, this);
}

std::unique_ptr<CandidateBase> TemplateLiteralMatcher::extractCandidate(
    const MatchFinder::MatchResult &Result) {
  if (auto *TSpec =
          Result.Nodes.getNodeAs<clang::ClassTemplateSpecializationDecl>(
              vara::toString(CandidateType::TemplateLiteral))) {
    assert(TSpec);
    auto SL = TSpec->getPointOfInstantiation();
    if (SL.isValid() && SL.isFileID()) {
      auto PLoc = Result.SourceManager->getPresumedLoc(SL);

      llvm::SmallString<256> FileName(PLoc.getFilename());
      const uint32_t Lineno = PLoc.getLine();
      const uint32_t Colno = PLoc.getColumn();

      if (this->ProjectRoot.has_value()) {
        Result.SourceManager->getFileManager().makeAbsolutePath(FileName);
        std::filesystem::path const AbsoluteFilePath(FileName.c_str());

        auto RelativeFilePath = std::filesystem::relative(
            AbsoluteFilePath, this->ProjectRoot.value());

        FileName = RelativeFilePath.generic_string();
      }

      auto BaseLocation = vara::CodeLocation{FileName.c_str(), Lineno, Colno};

      const std::string TClassName = TSpec->getNameAsString();

      // We need to manually extract the relevant template parameter(s)
      const auto *TDeclParams =
          TSpec->getSpecializedTemplate()->getTemplateParameters();
      const auto &TSpecArgs = TSpec->getTemplateArgs();

      for (size_t ArgIndex = 0;
           ArgIndex < TDeclParams->size() && ArgIndex < TSpecArgs.size();
           ++ArgIndex) {
        if (TSpecArgs[ArgIndex].getKind() !=
            clang::TemplateArgument::ArgKind::Integral) {
          continue;
        }

        const std::string TParamName =
            TDeclParams->getParam(ArgIndex)->getNameAsString();

        return std::make_unique<TemplateLiteral>(
            BaseLocation.File, BaseLocation.Lineno, BaseLocation.Colno,
            TClassName, TParamName);
      }
    }
  }
  return nullptr;
}

std::vector<CodeLocation> TemplateLiteralMatcher::extractUsageLocations(
    const MatchFinder::MatchResult &Result) {
  // TODO
  return {};
}

std::vector<FunctionLocation> TemplateLiteralMatcher::extractFunctionLocations(
    const MatchFinder::MatchResult &Result) {
  return {};
}

} // namespace vara
