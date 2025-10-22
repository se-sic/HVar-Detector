//
// Created by abeltluk on 10/22/25.
//

#ifndef HC_TEMPLATE_LITERAL_MATCHER_H
#define HC_TEMPLATE_LITERAL_MATCHER_H
#include "vara/Matchers/MatcherBase.h"
#include <memory>

namespace vara {

class TemplateLiteralMatcher : public HCMatcherBase {
public:
  explicit TemplateLiteralMatcher(
      const std::optional<std::filesystem::path> &ProjectRoot = std::nullopt)
      : HCMatcherBase(std::move(ProjectRoot)) {}

  void registerMatcher(MatchFinder &Finder) override;
  CandidateType getCandidateType() const override {
    return CandidateType::TemplateLiteral;
  };

protected:
  std::unique_ptr<CandidateBase>
  extractCandidate(const MatchFinder::MatchResult &Result) override;
  std::vector<CodeLocation>
  extractUsageLocations(const MatchFinder::MatchResult &Result) override;
  std::vector<FunctionLocation>
  extractFunctionLocations(const MatchFinder::MatchResult &Result) override;
};

} // namespace vara
#endif // HC_TEMPLATE_LITERAL_MATCHER_H
