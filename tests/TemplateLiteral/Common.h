#ifndef HIDDEN_CONFIGURABILITY_TESTS_COMMON_H
#define HIDDEN_CONFIGURABILITY_TESTS_COMMON_H

template <int T> class IntLiteralTemplate {};
template <long T> class LongLiteralTemplate {};
template <unsigned T> class UnsignedLiteralTemplate {};
template <unsigned long T> class ULongLiteralTemplate {};
template <char T> class CharLiteral{};
template <wchar_t T> class WCharLiteral{};
template <bool T> class BoolLiteral{};

// Additional templates with multiple non-type template parameters.
// Used by MultipleArguments.cpp tests.
template <int A, int B>
class MultiIntTemplate {};

template <int N, char C>
class MixedTemplate {};

// Function template with multiple non-type arguments
template <int A, int B>
inline void fnMulti() {}

// Templates mixing a type parameter with non-type parameters
template <typename T, int N>
class TypeAndNonType {};

// Parameter pack templates
template <int... Ns>
class PackIntTemplate {};

template <typename T, int... Ns>
class PackMixedTemplate {};

// Float and Double literal template parameters only supported from Clang 18 onwards
//template <float T> class FloatLiteral{};
//template <double T> class DoubleLiteral{};

enum class EnumLiteral : int {
  Value0 = 0,
  Value1 = 1,
  Value2 = 2
};

template <EnumLiteral T> class EnumLiteralTemplate {};

// Function template for testing templated function instantiations
template <int N>
inline void foo() {}

// A class template with templated member functions, a templated static member
// function and a static data member which depends on the class template
// non-type parameter. This type is used in FunctionUses tests.
template <int N>
struct C {
  template <int M>
  void memberFunc() {}

  template <int M>
  static void staticMemberFunc() {}

  static const int value = N;
};

// Template with a default non-type template argument to test default-argument
// instantiations.
template <int N = 3>
class DefaultArgTemplate {};

#endif //HIDDEN_CONFIGURABILITY_TESTS_COMMON_H
