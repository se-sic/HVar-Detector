//
// Created by abeltluk on 7/16/24.
//

#ifndef HIDDEN_CONFIGURABILITY_MATCHERS_H
#define HIDDEN_CONFIGURABILITY_MATCHERS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/FrontendActions.h"

#include "vara/Candidates/CandidateBase.h"

namespace vara {
namespace ast_matchers {
using namespace clang::ast_matchers;

const auto AnyLiteralType =
    anyOf(integerLiteral(), floatLiteral(), stringLiteral(), characterLiteral(),
          compoundLiteralExpr(), cxxBoolLiteral(), userDefinedLiteral());

const auto GlobalConstVarMatcher = // Match all variable declarations
    traverse(
        clang::TK_IgnoreUnlessSpelledInSource,
        varDecl(allOf(
            hasType(hasUnqualifiedDesugaredType(builtinType())),
            hasType(isConstQualified()), hasInitializer(AnyLiteralType),
            anyOf(hasParent(namespaceDecl()), hasParent(translationUnitDecl())),
            unless(isExpansionInSystemHeader()))));

const auto LocalConstVarMatcher = // Match all variable declarations
    traverse(clang::TK_IgnoreUnlessSpelledInSource,
             varDecl(allOf(
                 // That use a built-in type
                 hasType(hasUnqualifiedDesugaredType(builtinType())),
                 // Only const qualified declarations
                 hasType(isConstQualified()),
                 // That are directly initialized with a literal expression
                 hasInitializer(AnyLiteralType),
                 // Ignore variable declarations in for-loop headers
                 unless(hasParent(declStmt(hasParent(forStmt())))),
                 // Ignore declarations that are global to the namespace or
                 // translation unit
                 unless(anyOf(hasParent(namespaceDecl()),
                              hasParent(translationUnitDecl()),
                              hasParent(recordDecl()))),
                 // Ignore includes from system headers
                 unless(isExpansionInSystemHeader()))));

const auto ConstFieldASTMatcher = // Match all variable declarations
    traverse(clang::TK_IgnoreUnlessSpelledInSource,
             fieldDecl(allOf(
                 // That use a built-in type
                 hasType(hasUnqualifiedDesugaredType(builtinType())),
                 // Only const qualified declarations
                 hasType(isConstQualified()),
                 // That are directly initialized with a literal expression
                 hasInClassInitializer(AnyLiteralType),
                 // Only match fields in classes
                 hasParent(cxxRecordDecl()),
                 // Ignore system headers
                 unless(isExpansionInSystemHeader()))));

const auto
    TemplateLiteralMatcher = // Match Template instantiations using literals
    classTemplateSpecializationDecl( // It is not possible to directly match the
                                     // template parameter itself
        traverse(clang::TK_IgnoreUnlessSpelledInSource,
                 allOf(hasAnyTemplateArgument(isIntegral()),
                       unless(isExpansionInSystemHeader()))));

const auto LiteralCompMatcher = // Match comparison expressions
    binaryOperation(allOf(anyOf(hasLHS(AnyLiteralType), hasRHS(AnyLiteralType)),
                          unless(isExpansionInSystemHeader())));

} // namespace ast_matchers
} // namespace vara

template <> struct llvm::yaml::CustomMappingTraits<vara::ResultMapTy> {
  static void inputOne(IO &io, StringRef key, vara::ResultMapTy &elem) {
    auto kind = vara::fromString(key.str());

    if (kind == vara::CandidateType::Unknown) {
      io.setError("Invalid value for configurability kind");
      return;
    }

    io.mapRequired(key.str().c_str(), elem[kind]);
  }

  static void output(IO &io, vara::ResultMapTy &elem) {
    for (auto &[key, value] : elem) {
      io.mapRequired(vara::toString(key).c_str(), value);
    }
  }
};

#endif // HIDDEN_CONFIGURABILITY_MATCHERS_H
