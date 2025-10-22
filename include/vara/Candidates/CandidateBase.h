//
// Created by abeltluk on 9/6/24.
//

#ifndef HIDDEN_CONFIGURABILITY_CANIDATES_H
#define HIDDEN_CONFIGURABILITY_CANIDATES_H
#include "vara/Candidates/CandidateTypes.h"
#include "llvm/Support/YAMLTraits.h"

#include <filesystem>
#include <optional>
#include <utility>

namespace vara {

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

using ResultMapTy = std::map<vara::CandidateType,
                             std::vector<std::unique_ptr<vara::CandidateBase>>>;

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

template <>
struct llvm::yaml::MappingTraits<std::unique_ptr<vara::CandidateBase>> {
  static void mapping(IO &io, std::unique_ptr<vara::CandidateBase> &baseInfo) {
    baseInfo->mapToYaml(io);
  }
};

LLVM_YAML_IS_SEQUENCE_VECTOR(std::unique_ptr<vara::CandidateBase>)
LLVM_YAML_IS_SEQUENCE_VECTOR(vara::CodeLocation)
#endif // HIDDEN_CONFIGURABILITY_CANIDATES_H
