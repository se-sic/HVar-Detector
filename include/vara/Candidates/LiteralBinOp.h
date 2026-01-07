//
// Created by abeltluk on 10/30/25.
//

#ifndef HIDDEN_CONFIGURABILITY_LITERALCOMP_H
#define HIDDEN_CONFIGURABILITY_LITERALCOMP_H
#include "CandidateBase.h"

#include <algorithm>

namespace vara {
class LiteralBinOp : public CandidateBase {
public:
  LiteralBinOp(std::string File, uint32_t Lineno, uint32_t Colno,
               std::string ComparisonType)
      : CandidateBase(std::move(File), Lineno, Colno),
        BinOperator(std::move(ComparisonType)) {}

  LiteralBinOp(CodeLocation DefinitionLocation, std::string ComparisonType)
      : CandidateBase(std::move(DefinitionLocation)),
        BinOperator(std::move(ComparisonType)) {}

  LiteralBinOp() = default;
  LiteralBinOp(const LiteralBinOp &) = default;
  ~LiteralBinOp() override = default;
  LiteralBinOp &operator=(const LiteralBinOp &) = default;

  std::string BinOperator;

protected:
  void mapAdditionalProperties(llvm::yaml::IO &io) override {
    io.mapRequired("Operator", this->BinOperator);
  }
};
} // namespace vara

#endif // HIDDEN_CONFIGURABILITY_LITERALCOMP_H
