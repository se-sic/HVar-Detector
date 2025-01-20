//
// Created by abeltluk on 9/6/24.
//

#ifndef LLVM_HIDDENVARIABILITYCANDIDATES_H
#define LLVM_HIDDENVARIABILITYCANDIDATES_H
namespace vara {
enum class CandidateType {
  GlobalConstVar,
  LocalConstVar,
  ConstField,
  TemplateLiteral,
  Unknown
};

class CandidateBase {
public:
  CandidateBase(std::string File, uint32_t Lineno, uint32_t Colno)
      : File(File), Lineno(Lineno), Colno(Colno) {}

  CandidateBase(){};

  CandidateBase(const CandidateBase &) = default;

  void mapToYaml(llvm::yaml::IO &io) {
    // Map common properties
    io.mapRequired("Filename", this->File);
    io.mapRequired("Lineno", this->Lineno);
    io.mapRequired("Colno", this->Colno);
    this->mapAdditionalProperties(io);
  }

  virtual ~CandidateBase(){};

  std::string File;
  uint32_t Lineno;
  uint32_t Colno;

protected:
  virtual void mapAdditionalProperties(llvm::yaml::IO &io){};

private:
};

class TemplateLiteral : public CandidateBase {
public:
  TemplateLiteral(std::string File, uint32_t Lineno, uint32_t Colno,
                  std::string TClassName, std::string TArgName)
      : CandidateBase(File, Lineno, Colno), TClassName(TClassName),
        TArgName(TArgName) {}

  TemplateLiteral(const TemplateLiteral &);

  std::string TClassName;
  std::string TArgName;

protected:
  virtual void mapAdditionalProperties(llvm::yaml::IO &io) override {
    io.mapRequired("TemplateClassName", this->TClassName);
    io.mapRequired("TemplateParameterName", this->TArgName);
  }

private:
};

class HiddenVariable : public CandidateBase {
public:
  HiddenVariable(std::string File, uint32_t Lineno, uint32_t Colno,
                 std::string QualType, std::string QualifiedName)
      : CandidateBase(File, Lineno, Colno), QualType(QualType),
        QualifiedName(QualifiedName) {}

  HiddenVariable(const HiddenVariable &);

  std::string QualType;
  std::string QualifiedName;

protected:
  virtual void mapAdditionalProperties(llvm::yaml::IO &io) override {
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
      : HiddenVariable(File, Lineno, Colno, QualType, QualifiedName),
        ClassName(ClassName) {}

  HiddenField(const HiddenField &Other) = default;

  std::string ClassName;

protected:
  virtual void mapAdditionalProperties(llvm::yaml::IO &io) override {
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
#endif // LLVM_HIDDENVARIABILITYCANDIDATES_H
