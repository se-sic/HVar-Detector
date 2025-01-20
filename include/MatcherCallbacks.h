//
// Created by abeltluk on 9/6/24.
//

#ifndef LLVM_HIDDENVARIABILITYMATCHERCALLBACKS_H
#define LLVM_HIDDENVARIABILITYMATCHERCALLBACKS_H
#include <filesystem>
#include <optional>

#include "Matchers.h"

namespace vara {
using namespace clang::ast_matchers;
using namespace clang::tooling;

class HiddenVariabilityMatcherCallback : public MatchFinder::MatchCallback {
public:
  HiddenVariabilityMatcherCallback(
      ExecutionContext &Context, vara::ResultMapTy &Results,
      std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
      : Context(Context), Results(Results), ProjectRoot(ProjectRoot) {}

  virtual void run(const MatchFinder::MatchResult &Result) override = 0;

protected:
  std::optional<vara::CandidateBase>
  parseLocationFromDecl(const clang::Decl *D, clang::SourceManager *SM) {
    if (D && D->getBeginLoc().isValid() && D->getBeginLoc().isFileID()) {

      // Parsing of file name, line and column taken from SourceLocation::print

      const clang::PresumedLoc PLoc = SM->getPresumedLoc(D->getBeginLoc());

      if (PLoc.isInvalid()) {
        return std::nullopt;
      }

      llvm::SmallString<256> FileName(PLoc.getFilename());
      const uint32_t Lineno = PLoc.getLine();
      const uint32_t Colno = PLoc.getColumn();

      if (this->ProjectRoot.has_value()) {
        SM->getFileManager().makeAbsolutePath(FileName);
        std::filesystem::path AbsoluteFilePath(FileName.c_str());

        auto RelativeFilePath = std::filesystem::relative(
            AbsoluteFilePath, this->ProjectRoot.value());

        FileName = RelativeFilePath.generic_string();
      }

      return vara::CandidateBase(FileName.c_str(), Lineno, Colno);
    }

    return std::nullopt;
  }

  ExecutionContext &Context;
  vara::ResultMapTy &Results;
  std::optional<std::filesystem::path> ProjectRoot;
};

template <vara::CandidateType CType>
class VarDeclMatcherCallback : public HiddenVariabilityMatcherCallback {
public:
  VarDeclMatcherCallback(
      ExecutionContext &Context, vara::ResultMapTy &Results,
      std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
      : HiddenVariabilityMatcherCallback(Context, Results, ProjectRoot) {
    // Ensure that there is at least an empty vector in the results map
    Results[CType];
  }

  virtual void run(const MatchFinder::MatchResult &Result) override {
    auto *D = Result.Nodes.getNodeAs<clang::VarDecl>(vara::toString(CType));
    assert(D);

    auto BaseLoc = this->parseLocationFromDecl(D, Result.SourceManager);

    if (!BaseLoc.has_value()) {
      return;
    }

    Results[CType].push_back(std::make_shared<vara::HiddenVariable>(
        BaseLoc.value().File, BaseLoc.value().Lineno, BaseLoc.value().Colno,
        D->getType().getDesugaredType(*Result.Context).getAsString(),
        D->getQualifiedNameAsString()));
  }
};

class FieldDeclMatcherCallback : public HiddenVariabilityMatcherCallback {
public:
  FieldDeclMatcherCallback(
      ExecutionContext &Context, vara::ResultMapTy &Results,
      std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
      : HiddenVariabilityMatcherCallback(Context, Results, ProjectRoot) {
    // Ensure that there is at least an empty vector in the results map
    Results[vara::CandidateType::ConstField];
  }

  virtual void run(const MatchFinder::MatchResult &Result) override;
};

class TemplateLiteralMatcherCallback : public HiddenVariabilityMatcherCallback {
public:
  TemplateLiteralMatcherCallback(
      ExecutionContext &Context, vara::ResultMapTy &Results,
      std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
      : HiddenVariabilityMatcherCallback(Context, Results, ProjectRoot) {
    // Ensure that there is at least an empty vector in the results map
    Results[vara::CandidateType::TemplateLiteral];
  }

  virtual void run(const MatchFinder::MatchResult &Result) override;
};
} // namespace vara
#endif // LLVM_HIDDENVARIABILITYMATCHERCALLBACKS_H
