//
// Created by abeltluk on 10/22/25.
//
#include "vara/Matchers/ConstVarMatcher.h"
#include "vara/Candidates/HiddenVariable.h"

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

} // namespace vara
