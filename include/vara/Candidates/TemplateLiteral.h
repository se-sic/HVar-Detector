//
// Created by abeltluk on 10/22/25.
//

#ifndef HC_TEMPLATE_LITERAL_H
#define HC_TEMPLATE_LITERAL_H

#include "vara/Candidates/CandidateBase.h"

namespace vara {
class TemplateLiteral : public CandidateBase {
public:
  TemplateLiteral(std::string File, uint32_t Lineno, uint32_t Colno,
                  std::string TClassName, std::string TArgName)
      : CandidateBase(std::move(File), Lineno, Colno),
        TClassName(std::move(TClassName)), TArgName(std::move(TArgName)) {}

  TemplateLiteral() = default;
  TemplateLiteral(const TemplateLiteral &) = default;
  ~TemplateLiteral() override = default;
  TemplateLiteral &operator=(const TemplateLiteral &) = default;

  std::string TClassName;
  std::string TArgName;

protected:
  void mapAdditionalProperties(llvm::yaml::IO &io) override {
    io.mapRequired("TemplateClassName", this->TClassName);
    io.mapRequired("TemplateParameterName", this->TArgName);
  }

private:
};
} // namespace vara

#endif // HC_TEMPLATE_LITERAL_H
