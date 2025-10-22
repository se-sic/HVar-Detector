//
// Created by abeltluk on 9/6/24.
//

#include "MatcherCallbacks.h"
#include "Candidates.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <cstddef>
#include <memory>

namespace vara {

std::unique_ptr<CandidateBase>
ConstVarMatcherBase::extractCandidate(const MatchFinder::MatchResult &Result) {
  if (const auto *D = Result.Nodes.getNodeAs<clang::VarDecl>(
          vara::toString(CandidateType::GlobalConstVar))) {
    assert(D);

    auto BaseLoc = parseLocation(
        D->getBeginLoc(), Result.Context->getSourceManager(), ProjectRoot);

    if (!BaseLoc.has_value()) {
      return nullptr;
    }

    return std::make_unique<HiddenVariable>(
        BaseLoc.value().File, BaseLoc.value().Lineno, BaseLoc.value().Colno,
        D->getType().getDesugaredType(*Result.Context).getAsString(),
        D->getQualifiedNameAsString());
  }

  return nullptr;
}

std::vector<CodeLocation> ConstVarMatcherBase::extractUsageLocations(
    const MatchFinder::MatchResult &Result) {
  std::vector<CodeLocation> UsageLocations;
  if (const auto *D = Result.Nodes.getNodeAs<clang::VarDecl>(
          vara::toString(CandidateType::GlobalConstVar))) {
    auto RefMatcher = declRefExpr(to(varDecl(equalsNode(D)))).bind("varRef");
    auto RefMatcherCB =
        std::make_unique<VariableUsageMatcher>(this->ProjectRoot);

    MatchFinder Finder;
    Finder.addMatcher(RefMatcher, RefMatcherCB.get());
    Finder.matchAST(*Result.Context);

    UsageLocations = std::move(RefMatcherCB->UsageLocations);
  }

  return UsageLocations;
}

std::vector<FunctionLocation> ConstVarMatcherBase::extractFunctionLocations(
    const MatchFinder::MatchResult &Result) {
  return {};
}

void ConstVarMatcherBase::VariableUsageMatcher::run(
    const MatchFinder::MatchResult &Result) {
  if (const auto *DRE = Result.Nodes.getNodeAs<clang::DeclRefExpr>("varRef")) {
    if (const auto *VD = llvm::dyn_cast<clang::VarDecl>(DRE->getDecl())) {
      auto DeclLocation = parseLocation(
          VD->getBeginLoc(), Result.Context->getSourceManager(), ProjectRoot);
      auto UsageLocation = parseLocation(
          DRE->getBeginLoc(), Result.Context->getSourceManager(), ProjectRoot);

      if (!(UsageLocation && DeclLocation)) {
        return;
      }

      UsageLocations.push_back(UsageLocation.value());
    }
  }
}

void ConstFieldMatcher::registerMatcher(MatchFinder &Finder) {
  auto FieldMatcher =
      vara::ConstFieldMatcher.bind(vara::toString(this->getCandidateType()));

  Finder.addMatcher(FieldMatcher, this);
}

std::unique_ptr<CandidateBase>
ConstFieldMatcher::extractCandidate(const MatchFinder::MatchResult &Result) {
  if (const auto *FieldDecl = Result.Nodes.getNodeAs<clang::FieldDecl>(
          vara::toString(CandidateType::ConstField))) {
    // Match Declaration
    if (FieldDecl->getBeginLoc().isValid() &&
        FieldDecl->getBeginLoc().isFileID()) {
      auto BaseLocation =
          parseLocation(FieldDecl->getBeginLoc(),
                        Result.Context->getSourceManager(), this->ProjectRoot);

      if (!BaseLocation.has_value()) {
        return nullptr;
      }

      std::string ClassName = "<unknown>";

      if (const auto *recDecl = FieldDecl->getParent()) {
        ClassName = recDecl->getNameAsString();
      }

      return std::make_unique<HiddenField>(
          BaseLocation.value().File, BaseLocation.value().Lineno,
          BaseLocation.value().Colno,
          FieldDecl->getType().getDesugaredType(*Result.Context).getAsString(),
          FieldDecl->getQualifiedNameAsString(), ClassName);
    }
  }
  return nullptr;
}

std::vector<CodeLocation> ConstFieldMatcher::extractUsageLocations(
    const MatchFinder::MatchResult &Result) {
  std::vector<CodeLocation> UsageLocations;

  if (const auto *FieldDecl = Result.Nodes.getNodeAs<clang::FieldDecl>(
          vara::toString(CandidateType::ConstField))) {
    auto UsageMatcher = memberExpr(member(fieldDecl(equalsNode(FieldDecl))))
                            .bind("memberUsage");
    auto UsageMatcherCB =
        std::make_unique<MemberUsageMatcher>(this->ProjectRoot);

    MatchFinder MF;
    MF.addMatcher(UsageMatcher, UsageMatcherCB.get());
    MF.matchAST(*Result.Context);

    UsageLocations = std::move(UsageMatcherCB->UsageLocations);
  }

  return UsageLocations;
}

std::vector<FunctionLocation> ConstFieldMatcher::extractFunctionLocations(
    const MatchFinder::MatchResult &Result) {
  return {};
}

void ConstFieldMatcher::MemberUsageMatcher::run(
    const MatchFinder::MatchResult &Result) {
  if (const auto *ME =
          Result.Nodes.getNodeAs<clang::MemberExpr>("memberUsage")) {
    const auto *FD = llvm::dyn_cast<clang::FieldDecl>(ME->getMemberDecl());

    if (!FD) {
      return;
    }

    auto UsageLocation = parseLocation(
        ME->getBeginLoc(), Result.Context->getSourceManager(), ProjectRoot);
    auto FieldLocation = parseLocation(
        FD->getBeginLoc(), Result.Context->getSourceManager(), ProjectRoot);

    if (!(UsageLocation.has_value() && FieldLocation.has_value())) {
      return;
    }

    UsageLocations.push_back(UsageLocation.value());
  }
}

void FieldDeclMatcherCallback::run(const MatchFinder::MatchResult &Result) {
  if (const auto *FieldDecl = Result.Nodes.getNodeAs<clang::FieldDecl>(
          vara::toString(CandidateType::ConstField))) {
    // Match Declaration
    if (FieldDecl->getBeginLoc().isValid() &&
        FieldDecl->getBeginLoc().isFileID()) {
      auto BaseLocation =
          parseLocation(FieldDecl->getBeginLoc(),
                        Result.Context->getSourceManager(), ProjectRoot);

      if (!BaseLocation.has_value()) {
        return;
      }

      std::string ClassName = "<unknown>";

      if (const auto *recDecl = FieldDecl->getParent()) {
        ClassName = recDecl->getNameAsString();
      }

      this->Locations[BaseLocation.value()] = HiddenField(
          BaseLocation.value().File, BaseLocation.value().Lineno,
          BaseLocation.value().Colno,
          FieldDecl->getType().getDesugaredType(*Result.Context).getAsString(),
          FieldDecl->getQualifiedNameAsString(), ClassName);

      auto UsageMatcher = memberExpr(member(fieldDecl(equalsNode(FieldDecl))))
                              .bind("memberUsage");
      MatchFinder MF;
      MF.addMatcher(UsageMatcher, this);
      MF.matchAST(*Result.Context);
    }
  } else if (const auto *ME =
                 Result.Nodes.getNodeAs<clang::MemberExpr>("memberUsage")) {
    const auto *FD = llvm::dyn_cast<clang::FieldDecl>(ME->getMemberDecl());

    if (!FD) {
      return;
    }

    auto UsageLocation = parseLocation(
        ME->getBeginLoc(), Result.Context->getSourceManager(), ProjectRoot);
    auto FieldLocation = parseLocation(
        FD->getBeginLoc(), Result.Context->getSourceManager(), ProjectRoot);

    if (!(UsageLocation.has_value() && FieldLocation.has_value())) {
      return;
    }

    if (Locations.find(FieldLocation.value()) == Locations.end()) {
      return;
    }

    Locations[FieldLocation.value()].UsageLocations.push_back(
        UsageLocation.value());
  }
}

void FieldDeclMatcherCallback::mapResults(vara::ResultMapTy &Results) {
  // Ensure that there is at least an empty vector in the results map
  Results[vara::CandidateType::ConstField];

  for (const auto &[_, HF] : Locations) {
    Results[vara::CandidateType::ConstField].emplace_back(
        std::make_shared<HiddenField>(HF));
  }
}

void TemplateLiteralMatcherCallback::run(
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

        Locations[BaseLocation] =
            TemplateLiteral(BaseLocation.File, BaseLocation.Lineno,
                            BaseLocation.Colno, TClassName, TParamName);
        // TODO: Matcher for usage locations
      }
    }
  }
  // TODO: Usage Locations
}

void TemplateLiteralMatcherCallback::mapResults(vara::ResultMapTy &Results) {
  // Ensure that there is at least an empty vector in the results map
  Results[vara::CandidateType::TemplateLiteral];

  for (const auto &[_, TL] : Locations) {
    Results[vara::CandidateType::TemplateLiteral].emplace_back(
        std::make_shared<TemplateLiteral>(TL));
  }
}

} // namespace vara
