//
// Created by abeltluk on 10/22/25.
//

#ifndef HC_HIDDEN_VARIABLE_H
#define HC_HIDDEN_VARIABLE_H
#include "vara/Candidates/CandidateBase.h"

namespace vara {
class HiddenVariable : public CandidateBase {
public:
  HiddenVariable(std::string File, uint32_t Lineno, uint32_t Colno,
                 std::string QualType, std::string QualifiedName)
      : CandidateBase(std::move(File), Lineno, Colno),
        QualType(std::move(QualType)), QualifiedName(std::move(QualifiedName)) {
  }

  HiddenVariable() = default;
  HiddenVariable(const HiddenVariable &) = default;
  ~HiddenVariable() override = default;
  HiddenVariable &operator=(const HiddenVariable &) = default;

  std::string QualType;
  std::string QualifiedName;

protected:
  void mapAdditionalProperties(llvm::yaml::IO &io) override {
    io.mapRequired("Type", QualType);
    io.mapRequired("VarName", this->QualifiedName);
  }

private:
};
} // namespace vara

#endif // HC_HIDDEN_VARIABLE_H
