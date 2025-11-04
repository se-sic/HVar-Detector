//
// Created by abeltluk on 10/30/25.
//
#include "vara/Matchers/LiteralCompMatcher.h"
#include "vara/Candidates/CandidateBase.h"
#include "vara/Candidates/LiteralComp.h"

#include <memory>

namespace vara {
std::unique_ptr<CandidateBase>
LiteralCompMatcher::extractCandidate(const MatchFinder::MatchResult &Result) {
  return this
      ->extractCandidateImpl<clang::BinaryOperator, clang::CXXOperatorCallExpr,
                             clang::CXXRewrittenBinaryOperator>(Result);
}

std::vector<CodeLocation> LiteralCompMatcher::extractUsageLocations(
    const MatchFinder::MatchResult &Result) {
  return {};
}

std::vector<FunctionLocation> LiteralCompMatcher::extractFunctionLocations(
    const MatchFinder::MatchResult &Result) {
  return {};
}
} // namespace vara
