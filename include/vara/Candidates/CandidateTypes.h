//
// Created by abeltluk on 10/22/25.
//

#ifndef HC_CANDIDATE_TYPES_H
#define HC_CANDIDATE_TYPES_H
#include "llvm/Support/YAMLTraits.h"

namespace vara {
enum class CandidateType : std::uint8_t {
  GlobalConstVar,
  LocalConstVar,
  ConstField,
  TemplateLiteral,
  Unknown
};
}

template <> struct llvm::yaml::ScalarEnumerationTraits<vara::CandidateType> {
  static void enumeration(IO &io, vara::CandidateType &value) {
    io.enumCase(value, "LocalConstVar", vara::CandidateType::LocalConstVar);
    io.enumCase(value, "GlobalConstVar", vara::CandidateType::GlobalConstVar);
    io.enumCase(value, "ConstField", vara::CandidateType::ConstField);
    io.enumCase(value, "TemplateLiteral", vara::CandidateType::TemplateLiteral);
    io.enumCase(value, "Unknown", vara::CandidateType::Unknown);
  }
};
#endif // HC_CANDIDATE_TYPES_H
