#ifndef HIDDEN_CONFIGURABILITY_TESTS_COMMON_H
#define HIDDEN_CONFIGURABILITY_TESTS_COMMON_H

template <int T> class IntLiteralTemplate {};
template <long T> class LongLiteralTemplate {};
template <unsigned T> class UnsignedLiteralTemplate {};
template <unsigned long T> class ULongLiteralTemplate {};
template <char T> class CharLiteral{};
template <wchar_t T> class WCharLiteral{};
template <bool T> class BoolLiteral{};

// Float and Double literal template parameters only supported from Clang 18 onwards
//template <float T> class FloatLiteral{};
//template <double T> class DoubleLiteral{};

enum class EnumLiteral : int {
  Value0 = 0,
  Value1 = 1,
  Value2 = 2
};

template <EnumLiteral T> class EnumLiteralTemplate {};


#endif //HIDDEN_CONFIGURABILITY_TESTS_COMMON_H