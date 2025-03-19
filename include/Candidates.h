#include <utility>

//
// Created by abeltluk on 9/6/24.
//

#ifndef LLVM_HIDDENVARIABILITYCANDIDATES_H
#define LLVM_HIDDENVARIABILITYCANDIDATES_H
namespace vara {
enum class CandidateType : std::uint8_t {
  GlobalConstVar,
  LocalConstVar,
  ConstField,
  TemplateLiteral,
  Unknown
};

struct CodeLocation {
  std::string File;
  uint32_t Lineno{};
  uint32_t Colno{};
};

class CandidateBase {
public:
  CandidateBase(std::string File, uint32_t Lineno, uint32_t Colno)
      : Location{std::move(File), Lineno, Colno} {}

  CandidateBase() = default;

  CandidateBase(const CandidateBase &) = default;
  virtual ~CandidateBase() = default;
  CandidateBase &operator=(const CandidateBase &) = default;

  void mapToYaml(llvm::yaml::IO &io) {
    // Map common properties
    io.mapRequired("Location", this->Location);
    this->mapAdditionalProperties(io);
  }

  CodeLocation Location;

protected:
  virtual void mapAdditionalProperties(llvm::yaml::IO &io){};

private:
};

class TemplateLiteral : public CandidateBase {
public:
  TemplateLiteral(std::string File, uint32_t Lineno, uint32_t Colno,
                  std::string TClassName, std::string TArgName)
      : CandidateBase(std::move(File), Lineno, Colno),
        TClassName(std::move(TClassName)), TArgName(std::move(TArgName)) {}

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

class HiddenVariable : public CandidateBase {
public:
  HiddenVariable(std::string File, uint32_t Lineno, uint32_t Colno,
                 std::string QualType, std::string QualifiedName)
      : CandidateBase(std::move(File), Lineno, Colno),
        QualType(std::move(QualType)), QualifiedName(std::move(QualifiedName)) {
  }

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

class HiddenField : public HiddenVariable {
public:
  HiddenField(std::string File, uint32_t Lineno, uint32_t Colno,
              std::string QualType, std::string QualifiedName,
              std::string ClassName)
      : HiddenVariable(std::move(File), Lineno, Colno, std::move(QualType),
                       std::move(QualifiedName)),
        ClassName(std::move(ClassName)) {}

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

using ResultMapTy = std::map<vara::CandidateType,
                             std::vector<std::shared_ptr<vara::CandidateBase>>>;

inline std::string toString(CandidateType Type) noexcept {
  switch (Type) {
  case CandidateType::GlobalConstVar:
    return "GlobalConstVar";
  case CandidateType::LocalConstVar:
    return "LocalConstVar";
  case CandidateType::ConstField:
    return "ConstField";
  case CandidateType::TemplateLiteral:
    return "TemplateLiteral";
  case CandidateType::Unknown:
    return "Unknown";
  }
}

inline CandidateType fromString(std::string s) {
  if (s == "GlobalConstVar") {
    return CandidateType::GlobalConstVar;
  }
  if (s == "LocalConstVar") {
    return CandidateType::LocalConstVar;
  }
  if (s == "ConstField") {
    return CandidateType::ConstField;
  }
  if (s == "TemplateLiteral") {
    return CandidateType::TemplateLiteral;
  }

  return CandidateType::Unknown;
}

} // namespace vara

template <> struct llvm::yaml::MappingTraits<vara::CodeLocation> {
  static void mapping(IO &io, vara::CodeLocation &codeLocation) {
    io.mapRequired("Filename", codeLocation.File);
    io.mapRequired("Lineno", codeLocation.Lineno);
    io.mapRequired("Colno", codeLocation.Colno);
  }
};
#endif // LLVM_HIDDENVARIABILITYCANDIDATES_H
