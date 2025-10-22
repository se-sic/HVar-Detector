//
// Created by abeltluk on 10/22/25.
//

#ifndef HC_CONST_VAR_MATCHER_H
#define HC_CONST_VAR_MATCHER_H
#include "vara/Matchers/MatcherBase.h"
namespace vara {
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
} // namespace vara
#endif // HC_CONST_VAR_MATCHER_H
