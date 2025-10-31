//
// Created by abeltluk on 10/30/25.
//

#ifndef HIDDEN_CONFIGURABILITY_LITERALCOMP_H
#define HIDDEN_CONFIGURABILITY_LITERALCOMP_H
#include "CandidateBase.h"

#include <algorithm>

namespace vara {
class LiteralComp : public CandidateBase {
public:
  LiteralComp(std::string File, uint32_t Lineno, uint32_t Colno,
              std::string ComparisonType)
      : CandidateBase(std::move(File), Lineno, Colno),
        ComparisonType(std::move(ComparisonType)) {}

  LiteralComp() = default;
  LiteralComp(const LiteralComp &) = default;
  ~LiteralComp() override = default;
  LiteralComp &operator=(const LiteralComp &) = default;

  std::string ComparisonType;

protected:
  void mapAdditionalProperties(llvm::yaml::IO &io) override {
    io.mapRequired("ComparisonType", this->ComparisonType);
  }
};
} // namespace vara

#endif // HIDDEN_CONFIGURABILITY_LITERALCOMP_H
