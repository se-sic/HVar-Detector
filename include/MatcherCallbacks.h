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
      ExecutionContext &Context,
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : Context(Context), ProjectRoot(ProjectRoot) {}

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

  virtual void mapResults(vara::ResultMapTy &Results) = 0;
  ExecutionContext &Context;
  std::optional<std::filesystem::path> ProjectRoot;

  virtual void registerMatcher(MatchFinder &Finder) = 0;

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

class ConstVarMatcher : public HCMatcherBase {
public:
  ConstVarMatcher(
      ExecutionContext &Context,
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HCMatcherBase(Context, ProjectRoot) {}

  void mapResults(vara::ResultMapTy &Results) override;

  void registerMatcher(MatchFinder &Finder) override {
    auto GlobalMatcher = GlobalConstVarMatcher.bind(
        vara::toString(CandidateType::GlobalConstVar));

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

class HiddenVariabilityMatcherCallback : public MatchFinder::MatchCallback {
public:
  HiddenVariabilityMatcherCallback(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : ProjectRoot(ProjectRoot) {}

  void run(const MatchFinder::MatchResult &Result) override = 0;

  virtual void mapResults(vara::ResultMapTy &Results) = 0;
  std::optional<std::filesystem::path> ProjectRoot;
};

template <vara::CandidateType CType>
class VarDeclMatcherCallback : public HiddenVariabilityMatcherCallback {
public:
  VarDeclMatcherCallback(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HiddenVariabilityMatcherCallback(ProjectRoot) {}

  void run(const MatchFinder::MatchResult &Result) override {
    if (const auto *D =
            Result.Nodes.getNodeAs<clang::VarDecl>(vara::toString(CType))) {
      assert(D);

      auto BaseLoc = parseLocation(
          D->getBeginLoc(), Result.Context->getSourceManager(), ProjectRoot);

      if (!BaseLoc.has_value()) {
        return;
      }

      Locations[BaseLoc.value()] = HiddenVariable(
          BaseLoc.value().File, BaseLoc.value().Lineno, BaseLoc.value().Colno,
          D->getType().getDesugaredType(*Result.Context).getAsString(),
          D->getQualifiedNameAsString());

      // Add Matcher for usages of variable
      auto RefMatcher = declRefExpr(to(varDecl(equalsNode(D)))).bind("varRef");
      MatchFinder Finder;
      Finder.addMatcher(RefMatcher, this);
      Finder.matchAST(*Result.Context);

    } else if (const auto *DRE =
                   Result.Nodes.getNodeAs<clang::DeclRefExpr>("varRef")) {
      if (const auto *VD = llvm::dyn_cast<clang::VarDecl>(DRE->getDecl())) {
        auto DeclLocation = parseLocation(
            VD->getBeginLoc(), Result.Context->getSourceManager(), ProjectRoot);
        auto UsageLocation =
            parseLocation(DRE->getBeginLoc(),
                          Result.Context->getSourceManager(), ProjectRoot);

        if (!(UsageLocation && DeclLocation)) {
          return;
        }

        if (Locations.find(DeclLocation.value()) == Locations.end()) {
          return;
        }

        Locations[DeclLocation.value()].UsageLocations.push_back(
            UsageLocation.value());
      }
    }
  }

  void mapResults(vara::ResultMapTy &Results) override {
    Results[CType];

    for (const auto &[_, HiddenVar] : Locations) {
      Results[CType].push_back(
          std::make_shared<vara::HiddenVariable>(HiddenVar));
    }
  };

private:
  std::map<const CodeLocation, HiddenVariable> Locations;
};

class FieldDeclMatcherCallback : public HiddenVariabilityMatcherCallback {
public:
  FieldDeclMatcherCallback(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HiddenVariabilityMatcherCallback(ProjectRoot) {}

  void run(const MatchFinder::MatchResult &Result) override;

  void mapResults(vara::ResultMapTy &Results) override;

private:
  std::map<const CodeLocation, HiddenField> Locations;
};

class TemplateLiteralMatcherCallback : public HiddenVariabilityMatcherCallback {
public:
  TemplateLiteralMatcherCallback(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HiddenVariabilityMatcherCallback(ProjectRoot) {}

  void run(const MatchFinder::MatchResult &Result) override;

  void mapResults(vara::ResultMapTy &Results) override;

private:
  std::map<const CodeLocation, TemplateLiteral> Locations;
};
} // namespace vara
#endif // HIDDEN_CONFIGURABILITY_MATCHER_CALLBACKS_H
