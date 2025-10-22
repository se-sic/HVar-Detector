//
// Created by abeltluk on 10/22/25.
//

#include "vara/Matchers/ConstFieldMatcher.h"
#include "vara/Candidates/HiddenField.h"
#include "vara/Matchers/ASTMatchers.h"

namespace vara {

void ConstFieldMatcher::registerMatcher(MatchFinder &Finder) {
  auto FieldMatcher = ast_matchers::ConstFieldASTMatcher.bind(
      vara::toString(this->getCandidateType()));

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

} // namespace vara
