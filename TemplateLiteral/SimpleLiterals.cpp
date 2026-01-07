// Some simple template literal examples

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s
// CHECK: GlobalConstVar:
// CHECK-SAME: []
// CHECK: LocalConstVar:
// CHECK-SAME: []
// CHECK: ConstField:
// CHECK-SAME: []
class string;
#include "Common.h"
template <typename T> struct D{};

template <>
struct D<string> {
  static const int value = 100;
};


D<int> d;
IntLiteralTemplate<5> c1;
// Instantiate with different basic types
IntLiteralTemplate<5u> c2;
IntLiteralTemplate<5l> c3;
IntLiteralTemplate<5UL> c4;
IntLiteralTemplate<0x5> c5;
IntLiteralTemplate<0b101> c6;
IntLiteralTemplate<012> c7;
IntLiteralTemplate<-5> c8;
IntLiteralTemplate<5> c9;
// CHECK: TemplateLiteral:
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 22
// CHECK: TemplateClassName
// CHECK-SAME: IntLiteralTemplate
// CHECK: TemplateParameterName
// CHECK-SAME: T

// c2
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 24

// c3
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 25

// c4
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 26

// c5
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 27

// c6
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 28

// c7
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 29

// c8
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 30

// c9
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 31



// Additional template classes that accept literal non-type template parameters



// Instantiate with character and boolean literals
CharLiteral<'a'> ch1;
CharLiteral<'\n'> ch2;
BoolLiteral<true> b1;
BoolLiteral<false> b2;

// CHECK for ch1 (instantiation CharLiteral<'a'> at line 100)
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 97
// CHECK: TemplateClassName
// CHECK-SAME: CharLiteral

// CHECK for ch2 (instantiation CharLiteral<'\\n'> at line 101)
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 98
// CHECK: TemplateClassName
// CHECK-SAME: CharLiteral

// CHECK for b1 (instantiation BoolLiteral<true> at line 102)
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 99
// CHECK: TemplateClassName
// CHECK-SAME: BoolLiteral

// CHECK for b2 (instantiation BoolLiteral<false> at line 103)
// CHECK: Filename
// CHECK-SAME: SimpleLiterals.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 100
// CHECK: TemplateClassName
// CHECK-SAME: BoolLiteral