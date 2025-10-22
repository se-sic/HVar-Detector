//
// Created by abeltluk on 10/22/25.
//

#ifndef HC_HIDDEN_FIELD_H
#define HC_HIDDEN_FIELD_H
#include "vara/Candidates/HiddenVariable.h"

namespace vara {
class HiddenField : public HiddenVariable {
public:
  HiddenField(std::string File, uint32_t Lineno, uint32_t Colno,
              std::string QualType, std::string QualifiedName,
              std::string ClassName)
      : HiddenVariable(std::move(File), Lineno, Colno, std::move(QualType),
                       std::move(QualifiedName)),
        ClassName(std::move(ClassName)) {}

  HiddenField() = default;
  HiddenField(const HiddenField &Other) = default;
  ~HiddenField() override = default;
  HiddenField &operator=(const HiddenField &Other) = default;

  std::string ClassName;

protected:
  void mapAdditionalProperties(llvm::yaml::IO &io) override {
    HiddenVariable::mapAdditionalProperties(io);
    io.mapRequired("ClassName", this->ClassName);
  }

private:
};
} // namespace vara

#endif // HC_HIDDEN_FIELD_H
