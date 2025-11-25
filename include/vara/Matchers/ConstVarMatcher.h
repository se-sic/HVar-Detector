//
// Created by abeltluk on 10/22/25.
//

#ifndef HC_CONST_VAR_MATCHER_H
#define HC_CONST_VAR_MATCHER_H
#include "vara/Candidates/CandidateTypes.h"
#include "vara/Matchers/ASTMatchers.h"
#include "vara/Matchers/MatcherBase.h"

namespace vara {

namespace detail {
template <CandidateType C>
concept IsValidConstVarType = (C == CandidateType::LocalConstVar ||
                               C == CandidateType::GlobalConstVar);
}

class ConstVarMatcherBase : public HCMatcherBase {
public:
  explicit ConstVarMatcherBase(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HCMatcherBase(ProjectRoot) {}

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
    explicit VariableUsageMatcher(
        std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
        : ProjectRoot(std::move(ProjectRoot)) {}

    void run(const MatchFinder::MatchResult &Result) override;

    std::vector<CodeLocation> UsageLocations;

  private:
    std::optional<std::filesystem::path> ProjectRoot;
  };
};

template <CandidateType CType>
  requires detail::IsValidConstVarType<CType>
class ConstVarMatcher : public ConstVarMatcherBase {

public:
  explicit ConstVarMatcher(
      std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
      : ConstVarMatcherBase(std::move(ProjectRoot)) {}

  void registerMatcher(MatchFinder &Finder) override {
    if constexpr (CType == CandidateType::LocalConstVar) {
      auto LocalMatcher = ast_matchers::LocalConstVarMatcher.bind(
          vara::toString(getCandidateType()));

      Finder.addMatcher(LocalMatcher, this);
      return;
    } else if constexpr (CType == CandidateType::GlobalConstVar) {
      // Handled below
      auto GlobalMatcher = ast_matchers::GlobalConstVarMatcher.bind(
          vara::toString(getCandidateType()));

      Finder.addMatcher(GlobalMatcher, this);
    }
  };

  virtual CandidateType getCandidateType() const override { return CType; }
};
} // namespace vara
#endif // HC_CONST_VAR_MATCHER_H
