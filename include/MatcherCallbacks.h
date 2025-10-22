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

#include "Candidates.h"
#include "Matchers.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "llvm/Support/Casting.h"

namespace vara {
using namespace clang::ast_matchers;
using namespace clang::tooling;

using DynMatcherInterface = clang::ast_matchers::internal::DynMatcherInterface;

class HCMatcherBase : public MatchFinder::MatchCallback {
public:
  HCMatcherBase(
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

class ConstVarMatcherBase : public HCMatcherBase {
public:
  ConstVarMatcherBase(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HCMatcherBase(ProjectRoot) {}

  void registerMatcher(MatchFinder &Finder) override {
    auto GlobalMatcher = ast_matchers::GlobalConstVarMatcher.bind(
        vara::toString(getCandidateType()));

    Finder.addMatcher(GlobalMatcher, this);
  };

protected:
  std::unique_ptr<CandidateBase>
  extractCandidate(const MatchFinder::MatchResult &Result) override;
  std::vector<CodeLocation>
  extractUsageLocations(const MatchFinder::MatchResult &Result) override;

  std::vector<FunctionLocation>
  extractFunctionLocations(const MatchFinder::MatchResult &Result) override;

private:
  class VariableUsageMatcher : public MatchCallback {
  public:
    VariableUsageMatcher(
        std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
        : ProjectRoot(std::move(ProjectRoot)) {}

    void run(const MatchFinder::MatchResult &Result) override;

    std::vector<CodeLocation> UsageLocations;

  private:
    std::optional<std::filesystem::path> ProjectRoot;
  };
};

template <CandidateType CType>
class ConstVarMatcher : public ConstVarMatcherBase {
public:
  ConstVarMatcher(
      std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
      : ConstVarMatcherBase(std::move(ProjectRoot)) {}

  virtual CandidateType getCandidateType() const override { return CType; }
};

class ConstFieldMatcher : public HCMatcherBase {
public:
  ConstFieldMatcher(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HCMatcherBase(ProjectRoot) {}

  void registerMatcher(MatchFinder &Finder) override;
  CandidateType getCandidateType() const override {
    return CandidateType::ConstField;
  };

protected:
  std::unique_ptr<CandidateBase>
  extractCandidate(const MatchFinder::MatchResult &Result) override;
  std::vector<CodeLocation>
  extractUsageLocations(const MatchFinder::MatchResult &Result) override;
  std::vector<FunctionLocation>
  extractFunctionLocations(const MatchFinder::MatchResult &Result) override;

private:
  class MemberUsageMatcher : public MatchCallback {
  public:
    MemberUsageMatcher(
        std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
        : ProjectRoot(std::move(ProjectRoot)) {}

    virtual void run(const MatchFinder::MatchResult &Result) override;

    std::vector<CodeLocation> UsageLocations;

  private:
    std::optional<std::filesystem::path> ProjectRoot = std::nullopt;
  };
};

class TemplateLiteralMatcher : public HCMatcherBase {
public:
  TemplateLiteralMatcher(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HCMatcherBase(std::move(ProjectRoot)) {}

  void registerMatcher(MatchFinder &Finder) override;
  CandidateType getCandidateType() const override {
    return CandidateType::TemplateLiteral;
  };

protected:
  std::unique_ptr<CandidateBase>
  extractCandidate(const MatchFinder::MatchResult &Result) override;
  std::vector<CodeLocation>
  extractUsageLocations(const MatchFinder::MatchResult &Result) override;
  std::vector<FunctionLocation>
  extractFunctionLocations(const MatchFinder::MatchResult &Result) override;
};

} // namespace vara
#endif // HIDDEN_CONFIGURABILITY_MATCHER_CALLBACKS_H
