// Tests detection of Template classes with different literal types

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s
// CHECK: GlobalConstVar:
// CHECK-SAME: []
// CHECK: LocalConstVar:
// CHECK-SAME: []
// CHECK: ConstField:
// CHECK-SAME: []

#include "Common.h"

// Instantiate templates with different literal types
IntLiteralTemplate<42> it1;
// CHECK: Filename
// CHECK-SAME: DifferentLiteralTypes.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 15
// CHECK: TemplateClassName
// CHECK-SAME: IntLiteralTemplate

LongLiteralTemplate<42l> lt1;
// CHECK: Filename
// CHECK-SAME: DifferentLiteralTypes.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 23
// CHECK: TemplateClassName
// CHECK-SAME: LongLiteralTemplate

UnsignedLiteralTemplate<42u> ut1;
// CHECK: Filename
// CHECK-SAME: DifferentLiteralTypes.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 31
// CHECK: TemplateClassName
// CHECK-SAME: UnsignedLiteralTemplate

ULongLiteralTemplate<42UL> ult1;
// CHECK: Filename
// CHECK-SAME: DifferentLiteralTypes.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 39
// CHECK: TemplateClassName
// CHECK-SAME: ULongLiteralTemplate

CharLiteral<'x'> ch1;
// CHECK: Filename
// CHECK-SAME: DifferentLiteralTypes.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 47
// CHECK: TemplateClassName
// CHECK-SAME: CharLiteral

WCharLiteral<L'x'> wch1;
// CHECK: Filename
// CHECK-SAME: DifferentLiteralTypes.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 55
// CHECK: TemplateClassName
// CHECK-SAME: WCharLiteral

BoolLiteral<true> b1;
// CHECK: Filename
// CHECK-SAME: DifferentLiteralTypes.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 63
// CHECK: TemplateClassName
// CHECK-SAME: BoolLiteral

// FloatLiteral<3.14f> f1; // Disabled due to limited compiler support
// XCHECK: Filename
// XCHECK-SAME: DifferentLiteralTypes.cpp
// XCHECK-NEXT: Lineno
// XCHECK-SAME: 71
// XCHECK: TemplateClassName
// XCHECK-SAME: FloatLiteral

// DoubleLiteral<2.718> d1; // Disabled due to limited compiler support
// XCHECK: Filename
// XCHECK-SAME: DifferentLiteralTypes.cpp
// XCHECK-NEXT: Lineno
// XCHECK-SAME: 79
// XCHECK: TemplateClassName
// XCHECK-SAME: DoubleLiteral

EnumLiteralTemplate<EnumLiteral::Value1> el1;
// CHECK: Filename
// CHECK-SAME: DifferentLiteralTypes.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 87
// CHECK: TemplateClassName
// CHECK-SAME: EnumLiteralTemplate
