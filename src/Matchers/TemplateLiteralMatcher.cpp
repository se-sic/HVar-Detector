//
// Created by abeltluk on 10/22/25.
//
#include "vara/Matchers/TemplateLiteralMatcher.h"
#include "vara/Candidates/CandidateTypes.h"
#include "vara/Candidates/TemplateLiteral.h"
#include "vara/Matchers/ASTMatchers.h"
#include "clang/AST/TypeLoc.h"

namespace vara {

void TemplateLiteralMatcher::registerMatcher(MatchFinder &Finder) {
  auto TemplateMatcher = ast_matchers::TemplateLiteralASTMatcher.bind(
      toString(this->getCandidateType()));

  Finder.addMatcher(TemplateMatcher, this);
}

std::unique_ptr<CandidateBase> TemplateLiteralMatcher::extractCandidate(
    const MatchFinder::MatchResult &Result) {

  // First, handle a syntactic template-id match (one per instantiation site).
  if (auto *TSLoc =
          Result.Nodes.getNodeAs<clang::TemplateSpecializationTypeLoc>(
              toString(CandidateType::TemplateLiteral))) {
    // Get the underlying TemplateSpecializationType
    auto *TST = TSLoc->getTypePtr();
    if (!TST) {
      return nullptr;
    }

    // Resolve template declaration for class name and parameter names
    auto TemplateName = TST->getTemplateName();
    auto *TemplateDecl = TemplateName.getAsTemplateDecl();
    if (!TemplateDecl) {
      return nullptr;
    }
    const auto *Templated = TemplateDecl->getTemplatedDecl();
    const std::string TClassName =
        Templated ? Templated->getNameAsString() : std::string();

    // Iterate TemplateArgumentLocs to find non-type integral arguments.
    for (unsigned ArgIdx = 0; ArgIdx < TSLoc->getNumArgs(); ++ArgIdx) {
      const auto &Arg = TST->getArg(ArgIdx);
      const auto ArgLocInfo = TSLoc->getArgLoc(ArgIdx);

      if (!this->isIntegralLiteralTemplateArg(Arg, ArgLocInfo, Result)) {
        continue;
      }

      const auto ArgLoc = TSLoc->getArgLoc(ArgIdx).getSourceRange().getBegin();
      if (!ArgLoc.isValid() || !ArgLoc.isFileID()) {
        continue;
      }

      auto PLoc = Result.SourceManager->getPresumedLoc(ArgLoc);
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

      auto BaseLocation = CodeLocation{FileName.c_str(), Lineno, Colno};

      // Map template parameter name (if available)
      const auto *TDeclParams = TemplateDecl->getTemplateParameters();
      std::string TParamName;
      if (TDeclParams && ArgIdx < TDeclParams->size())
        TParamName = TDeclParams->getParam(ArgIdx)->getNameAsString();

      return std::make_unique<TemplateLiteral>(
          BaseLocation.File, BaseLocation.Lineno, BaseLocation.Colno,
          TClassName, TParamName);
    }
  }

  return nullptr;
}

std::vector<CodeLocation> TemplateLiteralMatcher::extractUsageLocations(
    const MatchFinder::MatchResult &Result) {
  // Generally, UsageLocations should cover all places in the template class
  // definition where the literal template parameter is used.

  // Additionally, there may be other UseLocations, such as:
  // 1. Variable declaration of templated type with literal parameter (e.g.
  // std::array<int, 5> arr;)
  //    -> UsageLocations of the declared variable (Similar to
  //    ConstVarMatcher/ConstFieldMatcher)
  // 2. Direct uses in expressions (e.g. sizeof(TemplatedClass<5>))
  //    -> No additional UsageLocations
  // 3. Templated functions with literal parameters (e.g. template<int N> void
  // func() {...}; func<5>();)
  //    -> Might want a separate matcher to match by specialization and then
  //    extract usage locations.

  // For now, just handle the general case
  std::vector<CodeLocation> UsageLocations;

  if (auto *TSLoc =
          Result.Nodes.getNodeAs<clang::TemplateSpecializationTypeLoc>(
              toString(CandidateType::TemplateLiteral))) {
    auto *TST = TSLoc->getTypePtr();
    if (!TST) {
      return {};
    }

    // Resolve template declaration for class name and parameter names
    auto TemplateName = TST->getTemplateName();
    auto *TemplateDecl = TemplateName.getAsTemplateDecl();
    if (!TemplateDecl) {
      return {};
    }
    const auto *Templated = TemplateDecl->getTemplatedDecl();

    if (!Templated) {
      return {};
    }

    const auto *TDeclParams = TemplateDecl->getTemplateParameters();

    // Iterate over all template arguments to identify integral literal
    // arguments
    for (unsigned ArgIdx = 0; ArgIdx < TSLoc->getNumArgs(); ++ArgIdx) {
      if (ArgIdx >= TDeclParams->size()) {
        return UsageLocations;
      }

      const auto &Arg = TSLoc->getTypePtr()->getArg(ArgIdx);
      const auto ArgLocInfo = TSLoc->getArgLoc(ArgIdx);

      if (!this->isIntegralLiteralTemplateArg(Arg, ArgLocInfo, Result)) {
        continue;
      }

      const auto *NTParam = llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(
          TDeclParams->getParam(ArgIdx));
      if (!NTParam) {
        continue;
      }

      auto ParamUseMatcher =
          declRefExpr(to(nonTypeTemplateParmDecl(equalsNode(NTParam))))
              .bind("paramUse");
      MatchFinder Finder;

      // TODO
    }
  }

  return UsageLocations;
}

std::vector<FunctionLocation> TemplateLiteralMatcher::extractFunctionLocations(
    const MatchFinder::MatchResult &Result) {
  return {};
}

bool TemplateLiteralMatcher::isIntegralLiteralTemplateArg(
    const clang::TemplateArgument &Arg,
    const clang::TemplateArgumentLoc &ArgLocInfo,
    const MatchFinder::MatchResult &Result) {
  bool isIntegralLiteral = false;
  if (Arg.getKind() == clang::TemplateArgument::Integral) {
    isIntegralLiteral = true;
  } else {
    if (ArgLocInfo.getArgument().getKind() ==
        clang::TemplateArgument::Expression) {
      const clang::Expr *ArgExpr = ArgLocInfo.getArgument().getAsExpr();
      if (ArgExpr) {
        if (ArgExpr->isCXX98IntegralConstantExpr(*Result.Context) ||
            ArgExpr->isCXX11ConstantExpr(*Result.Context)) {
          isIntegralLiteral = true;
        }
      }
    }
  }

  return isIntegralLiteral;
}
} // namespace vara
