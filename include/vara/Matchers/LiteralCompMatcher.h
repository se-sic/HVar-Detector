#ifndef HIDDEN_CONFIGURABILITY_LITERALCOMPMATCHER_H
#define HIDDEN_CONFIGURABILITY_LITERALCOMPMATCHER_H
#include <filesystem>
#include <optional>

#include "MatcherBase.h"
#include "vara/Candidates/LiteralComp.h"
#include "vara/Matchers/ASTMatchers.h"
#include "clang/Basic/OperatorKinds.h"

namespace vara {
class LiteralCompMatcher : public HCMatcherBase {
public:
  explicit LiteralCompMatcher(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HCMatcherBase(ProjectRoot) {}

  void registerMatcher(MatchFinder &Finder) override {
    auto LiteralCompMatcher = ast_matchers::LiteralCompASTMatcher.bind(
        vara::toString(getCandidateType()));

    Finder.addMatcher(LiteralCompMatcher, this);
  }

  CandidateType getCandidateType() const override {
    return CandidateType::LiteralComp;
  };

protected:
  std::unique_ptr<CandidateBase>
  extractCandidate(const MatchFinder::MatchResult &Result) override;
  std::vector<CodeLocation>
  extractUsageLocations(const MatchFinder::MatchResult &Result) override;
  std::vector<FunctionLocation>
  extractFunctionLocations(const MatchFinder::MatchResult &Result) override;

private:
  template <typename OpT>
  std::unique_ptr<LiteralComp>
  handleOperator(const OpT *Op, const MatchFinder::MatchResult &Result) {
    auto BaseLoc = parseLocation(
        Op->getOperatorLoc(), Result.Context->getSourceManager(), ProjectRoot);

    if (!BaseLoc.has_value()) {
      return nullptr;
    }

    std::string OpStr;
    if constexpr (std::is_same_v<OpT, clang::BinaryOperator>) {
      OpStr = clang::BinaryOperator::getOpcodeStr(Op->getOpcode()).str();
    } else if constexpr (std::is_same_v<OpT, clang::CXXOperatorCallExpr>) {
      OpStr = clang::getOperatorSpelling(Op->getOperator());
    } else if constexpr (std::is_same_v<OpT,
                                        clang::CXXRewrittenBinaryOperator>) {
      OpStr = clang::BinaryOperator::getOpcodeStr(Op->getOpcode()).str();
    } else {
      return nullptr;
    }

    return std::make_unique<LiteralComp>(BaseLoc.value(), OpStr);
  }

  template <typename NodeT, typename... OtherTs>
  std::unique_ptr<LiteralComp>
  extractCandidateImpl(const MatchFinder::MatchResult &Result) {
    if (const auto *Node = Result.Nodes.getNodeAs<NodeT>(
            vara::toString(CandidateType::LiteralComp))) {
      return handleOperator(Node, Result);
    }
    if constexpr (sizeof...(OtherTs) > 0) {
      return this->extractCandidateImpl<OtherTs...>(Result);
    }
    return nullptr;
  }
};

} // namespace vara

#endif // HIDDEN_CONFIGURABILITY_LITERALCOMPMATCHER_H
