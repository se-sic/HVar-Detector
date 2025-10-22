//
// Created by abeltluk on 10/22/25.
//

#ifndef HC_CONST_FIELD_MATCHER_H
#define HC_CONST_FIELD_MATCHER_H
#include "vara/Matchers/MatcherBase.h"

namespace vara {

class ConstFieldMatcher : public HCMatcherBase {
public:
  explicit ConstFieldMatcher(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HCMatcherBase(ProjectRoot) {}

  void registerMatcher(MatchFinder &Finder) override;
  CandidateType getCandidateType() const override {
    return CandidateType::ConstField;
  };

protected:
  std::unique_ptr<CandidateBase>
  extractCandidate(const MatchFinder::MatchResult &Result) override;
  std::vector<CodeLocation>
  extractUsageLocations(const MatchFinder::MatchResult &Result) override;
  std::vector<FunctionLocation>
  extractFunctionLocations(const MatchFinder::MatchResult &Result) override;

private:
  class MemberUsageMatcher : public MatchCallback {
  public:
    explicit MemberUsageMatcher(
        std::optional<std::filesystem::path> ProjectRoot = std::nullopt)
        : ProjectRoot(std::move(ProjectRoot)) {}

    virtual void run(const MatchFinder::MatchResult &Result) override;

    std::vector<CodeLocation> UsageLocations;

  private:
    std::optional<std::filesystem::path> ProjectRoot = std::nullopt;
  };
};

} // namespace vara

#endif // HC_CONST_FIELD_MATCHER_H
