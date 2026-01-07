//
// Created by abeltluk on 10/30/25.
//
#include "vara/Matchers/LiteralBinOpMatcher.h"
#include "vara/Candidates/CandidateBase.h"
#include "vara/Candidates/LiteralBinOp.h"

#include <memory>
#include <vector>

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Stmt.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"

namespace vara {
std::unique_ptr<CandidateBase>
LiteralBinOpMatcher::extractCandidate(const MatchFinder::MatchResult &Result) {
  return this
      ->extractCandidateImpl<clang::BinaryOperator, clang::CXXOperatorCallExpr,
                             clang::CXXRewrittenBinaryOperator>(Result);
}

std::vector<CodeLocation> LiteralBinOpMatcher::extractUsageLocations(
    const MatchFinder::MatchResult &Result) {
  return {};
}

std::vector<FunctionLocation> LiteralBinOpMatcher::extractFunctionLocations(
    const MatchFinder::MatchResult &Result) {
  std::vector<FunctionLocation> Functions;

  // Try to get the matched expression node (could be different stmt types)
  const clang::Stmt *MatchedStmt = nullptr;
  if (auto *BO = Result.Nodes.getNodeAs<clang::BinaryOperator>(
          vara::toString(getCandidateType()))) {
    MatchedStmt = BO;
  } else if (auto *CC = Result.Nodes.getNodeAs<clang::CXXOperatorCallExpr>(
                 vara::toString(getCandidateType()))) {
    MatchedStmt = CC;
  } else if (auto *RW =
                 Result.Nodes.getNodeAs<clang::CXXRewrittenBinaryOperator>(
                     vara::toString(getCandidateType()))) {
    MatchedStmt = RW;
  }

  if (!MatchedStmt || !Result.Context) {
    return Functions;
  }

  // Breadth-first walk of parent DynTypedNodes until we find an enclosing
  // FunctionDecl/CXXMethodDecl.
  auto Parents = Result.Context->getParents(*MatchedStmt);
  llvm::SmallVector<clang::DynTypedNode, 32> Worklist(
      llvm::iterator_range(Parents.begin(), Parents.end()));

  llvm::SmallPtrSet<const void *, 32> Visited;

  while (!Worklist.empty()) {
    auto Node = Worklist.pop_back_val();

    if (const auto *FD = Node.get<clang::FunctionDecl>()) {
      Functions.push_back(FunctionLocation{FD->getQualifiedNameAsString()});
      return Functions;
    }
    if (const auto *MD = Node.get<clang::CXXMethodDecl>()) {
      Functions.push_back(FunctionLocation{MD->getQualifiedNameAsString()});
      return Functions;
    }

    // Prevent revisiting the same AST node
    const void *Key = Node.getMemoizationData();
    if (!Key) {
      continue;
    }
    if (!Visited.insert(Key).second) {
      continue;
    }

    // Enqueue parents of the current node
    for (const auto &P : Result.Context->getParents(Node)) {
      Worklist.push_back(P);
    }
  }

  return Functions;
}
} // namespace vara
