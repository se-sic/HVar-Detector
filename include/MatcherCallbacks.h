//
// Created by abeltluk on 9/6/24.
//

#ifndef LLVM_HIDDENVARIABILITYMATCHERCALLBACKS_H
#define LLVM_HIDDENVARIABILITYMATCHERCALLBACKS_H
#include <clang/Tooling/Execution.h>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>

#include "Matchers.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/Support/Casting.h"

namespace vara {
using namespace clang::ast_matchers;
using namespace clang::tooling;

class HiddenVariabilityMatcherCallback : public MatchFinder::MatchCallback {
public:
  HiddenVariabilityMatcherCallback(
      ExecutionContext &Context,
      std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
      : Context(Context), ProjectRoot(ProjectRoot) {}

  void run(const MatchFinder::MatchResult &Result) override = 0;

  virtual void mapResults(vara::ResultMapTy &Results) = 0;
  ExecutionContext &Context;
  std::optional<std::filesystem::path> ProjectRoot;
};

template <vara::CandidateType CType>
class VarDeclMatcherCallback : public HiddenVariabilityMatcherCallback {
public:
  VarDeclMatcherCallback(
      ExecutionContext &Context,
      std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
      : HiddenVariabilityMatcherCallback(Context, ProjectRoot) {}

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
      ExecutionContext &Context,
      std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
      : HiddenVariabilityMatcherCallback(Context, std::move(ProjectRoot)) {}

  void run(const MatchFinder::MatchResult &Result) override;

  void mapResults(vara::ResultMapTy &Results) override;

private:
  std::map<const CodeLocation, HiddenField> Locations;
};

class TemplateLiteralMatcherCallback : public HiddenVariabilityMatcherCallback {
public:
  TemplateLiteralMatcherCallback(
      ExecutionContext &Context,
      std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
      : HiddenVariabilityMatcherCallback(Context, ProjectRoot) {}

  void run(const MatchFinder::MatchResult &Result) override;

  void mapResults(vara::ResultMapTy &Results) override;

private:
  std::map<const CodeLocation, TemplateLiteral> Locations;
};
} // namespace vara
#endif // LLVM_HIDDENVARIABILITYMATCHERCALLBACKS_H
