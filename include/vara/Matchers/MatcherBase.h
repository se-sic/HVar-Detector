//
// Created by Lukas Abelt on 9/6/24.
//

#ifndef HIDDEN_CONFIGURABILITY_MATCHER_CALLBACKS_H
#define HIDDEN_CONFIGURABILITY_MATCHER_CALLBACKS_H
#include <clang/Tooling/Execution.h>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "vara/Candidates/CandidateBase.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"

namespace vara {
using namespace clang::ast_matchers;
using namespace clang::tooling;

using DynMatcherInterface = clang::ast_matchers::internal::DynMatcherInterface;

class HCMatcherBase : public MatchFinder::MatchCallback {
public:
  explicit HCMatcherBase(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : ProjectRoot(ProjectRoot) {}

  void run(const MatchFinder::MatchResult &Result) override {
    // All matchers should follow the same overall logic:
    // 1. Extract candidate from match result
    // 2. Extract usage locations from match result
    // 3. Extract function locations from match result

    // Store candidate if extraction was successful
    if (auto Candidate = extractCandidate(Result)) {
      auto UsageLocations = extractUsageLocations(Result);
      Candidate->UsageLocations = std::move(UsageLocations);

      auto FunctionLocations = extractFunctionLocations(Result);
      Candidate->UsageFunctions = std::move(FunctionLocations);

      Matches[Candidate->DefinitionLocation] = std::move(Candidate);
    }
  }

  void mapResults(vara::ResultMapTy &Results, CandidateType Type) {
    Results[Type];

    for (auto &[_, HiddenVar] : Matches) {
      Results[Type].push_back(std::move(HiddenVar));
    }
  }

  std::optional<std::filesystem::path> ProjectRoot;

  virtual void registerMatcher(MatchFinder &Finder) = 0;

  virtual CandidateType getCandidateType() const = 0;

  using MatchMapTy = std::map<CodeLocation, std::unique_ptr<CandidateBase>>;

protected:
  virtual std::unique_ptr<CandidateBase>
  extractCandidate(const MatchFinder::MatchResult &Result) = 0;
  virtual std::vector<CodeLocation>
  extractUsageLocations(const MatchFinder::MatchResult &Result) = 0;
  virtual std::vector<FunctionLocation>
  extractFunctionLocations(const MatchFinder::MatchResult &Result) = 0;

private:
  MatchMapTy Matches;
};

} // namespace vara
#endif // HIDDEN_CONFIGURABILITY_MATCHER_CALLBACKS_H
