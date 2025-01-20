//
// Created by abeltluk on 9/6/24.
//

#include "MatcherCallbacks.h"

namespace vara {

void FieldDeclMatcherCallback::run(const MatchFinder::MatchResult &Result) {

  auto *FieldDecl = Result.Nodes.getNodeAs<clang::FieldDecl>(
      vara::toString(CandidateType::ConstField));
  assert(FieldDecl);
  if (FieldDecl->getBeginLoc().isValid() &&
      FieldDecl->getBeginLoc().isFileID()) {
    auto BaseLocation =
        this->parseLocationFromDecl(FieldDecl, Result.SourceManager);

    if (!BaseLocation.has_value()) {
      return;
    }

    std::string ClassName = "<unknown>";

    if (const auto *recDecl = FieldDecl->getParent()) {
      ClassName = recDecl->getNameAsString();
    }

    Results[CandidateType::ConstField].push_back(std::make_shared<HiddenField>(
        BaseLocation.value().File, BaseLocation.value().Lineno,
        BaseLocation.value().Colno,
        FieldDecl->getType().getDesugaredType(*Result.Context).getAsString(),
        FieldDecl->getQualifiedNameAsString(), ClassName));
  }
}

void TemplateLiteralMatcherCallback::run(
    const MatchFinder::MatchResult &Result) {
  auto *TSpec = Result.Nodes.getNodeAs<clang::ClassTemplateSpecializationDecl>(
      vara::toString(CandidateType::TemplateLiteral));
  assert(TSpec);
  auto SL = TSpec->getPointOfInstantiation();
  if (SL.isValid() && SL.isFileID()) {
    auto PLoc = Result.SourceManager->getPresumedLoc(SL);

    llvm::SmallString<256> FileName(PLoc.getFilename());
    const uint32_t Lineno = PLoc.getLine();
    const uint32_t Colno = PLoc.getColumn();

    if (this->ProjectRoot.has_value()) {
      Result.SourceManager->getFileManager().makeAbsolutePath(FileName);
      std::filesystem::path AbsoluteFilePath(FileName.c_str());

      auto RelativeFilePath = std::filesystem::relative(
          AbsoluteFilePath, this->ProjectRoot.value());

      FileName = RelativeFilePath.generic_string();
    }

    auto BaseLocation = vara::CandidateBase(FileName.c_str(), Lineno, Colno);

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

      Results[CandidateType::TemplateLiteral].push_back(
          std::make_shared<TemplateLiteral>(
              BaseLocation.File, BaseLocation.Lineno, BaseLocation.Colno,
              TClassName, TParamName));
    }
  }
}
} // namespace vara
