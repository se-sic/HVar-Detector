//
// Created by abeltluk on 7/16/24.
//

#ifndef HIDDENCONFIGURABILITYMATCHERS_H
#define HIDDENCONFIGURABILITYMATCHERS_H

#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Execution.h"
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Signals.h"

#include "Candidates.h"

namespace vara {

using namespace clang::ast_matchers;

const auto AnyLiteralType =
    anyOf(integerLiteral(), floatLiteral(), stringLiteral(), characterLiteral(),
          compoundLiteralExpr(), cxxBoolLiteral());

const auto GlobalConstVarMatcher = // Match all variable declarations
    traverse(
        clang::TK_IgnoreUnlessSpelledInSource,
        varDecl(allOf(
            // That use a built-in type
            hasType(hasUnqualifiedDesugaredType(builtinType())),
            // Only const qualified declarations
            hasType(isConstQualified()),
            // That are directly initialized with a literal expression
            hasInitializer(AnyLiteralType),
            // Only match declarations that are global to the namespace or
            // translation unit
            anyOf(hasParent(namespaceDecl()), hasParent(translationUnitDecl())),
            // Ignore system headers
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

const auto ConstFieldMatcher = // Match all variable declarations
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

} // namespace vara

template <>
struct llvm::yaml::MappingTraits<std::shared_ptr<vara::CandidateBase>> {
  static void mapping(IO &io, std::shared_ptr<vara::CandidateBase> &baseInfo) {
    baseInfo->mapToYaml(io);
  }
};

template <> struct llvm::yaml::ScalarEnumerationTraits<vara::CandidateType> {
  static void enumeration(IO &io, vara::CandidateType &value) {
    io.enumCase(value, "LocalConstVar", vara::CandidateType::LocalConstVar);
    io.enumCase(value, "GlobalConstVar", vara::CandidateType::GlobalConstVar);
    io.enumCase(value, "ConstField", vara::CandidateType::ConstField);
    io.enumCase(value, "TemplateLiteral", vara::CandidateType::TemplateLiteral);
    io.enumCase(value, "Unknown", vara::CandidateType::Unknown);
  }
};

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

LLVM_YAML_IS_SEQUENCE_VECTOR(std::shared_ptr<vara::CandidateBase>)
#endif // HIDDENCONFIGURABILITYMATCHERS_H
