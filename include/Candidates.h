//
// Created by abeltluk on 9/6/24.
//

#ifndef HIDDEN_CONFIGURABILITY_CANIDATES_H
#define HIDDEN_CONFIGURABILITY_CANIDATES_H
#include "llvm/Support/YAMLTraits.h"

#include <utility>

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

  friend bool operator<(const CodeLocation &lhs, const CodeLocation &rhs) {
    return std::tie(lhs.File, lhs.Lineno, lhs.Colno) <
           std::tie(rhs.File, rhs.Lineno, rhs.Colno);
  }
};

struct FunctionLocation {
  std::string FunctionName;
};

static std::optional<vara::CodeLocation>
parseLocation(const clang::SourceLocation &SL, clang::SourceManager &SM,
              std::optional<std::filesystem::path> ProjectRoot) {
  if (SL.isValid() && SL.isFileID()) {
    // Parsing of file name, line and column taken from SourceLocation::print

    clang::FullSourceLoc FLoc(SL, SM);
    if (FLoc.isInvalid()) {
      return std::nullopt;
    }

    llvm::SmallString<256> FileName(SM.getFilename(SL));
    const uint32_t Lineno = FLoc.getSpellingLineNumber();
    const uint32_t Colno = FLoc.getSpellingColumnNumber();

    if (ProjectRoot.has_value()) {
      SM.getFileManager().makeAbsolutePath(FileName);
      std::filesystem::path AbsoluteFilePath(FileName.c_str());

      auto RelativeFilePath =
          std::filesystem::relative(AbsoluteFilePath, ProjectRoot.value());

      FileName = RelativeFilePath.generic_string();
    }

    return vara::CodeLocation{FileName.c_str(), Lineno, Colno};
  }

  return std::nullopt;
}

class CandidateBase {
public:
  CandidateBase(std::string File, uint32_t Lineno, uint32_t Colno)
      : DefinitionLocation{std::move(File), Lineno, Colno} {}

  CandidateBase(CodeLocation DefinitionLocation,
                std::vector<CodeLocation> Usages)
      : DefinitionLocation{std::move(DefinitionLocation)},
        UsageLocations(std::move(Usages)) {}

  CandidateBase() = default;

  CandidateBase(const CandidateBase &) = default;
  virtual ~CandidateBase() = default;
  CandidateBase &operator=(const CandidateBase &) = default;

  void mapToYaml(llvm::yaml::IO &io) {
    // Map common properties
    io.mapRequired("Location", this->DefinitionLocation);
    this->mapAdditionalProperties(io);
    io.mapRequired("UseLocations", this->UsageLocations);
  }

  CodeLocation DefinitionLocation;
  std::vector<CodeLocation> UsageLocations;
  std::vector<FunctionLocation> UsageFunctions;

protected:
  virtual void mapAdditionalProperties(llvm::yaml::IO &io){};
};

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
  return "Unknown";
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

LLVM_YAML_IS_SEQUENCE_VECTOR(vara::CodeLocation)
#endif // HIDDEN_CONFIGURABILITY_CANIDATES_H
