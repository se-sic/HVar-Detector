// Test some very basic occurrences of global const vars

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s

const int Test1 = 1000;
// CHECK: GlobalConstVar:
// CHECK: Filename
// CHECK-SAME: BasicTests.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 6
// CHECK: Type
// CHECK-SAME: const int
// CHECK: VarName
// CHECK-SAME: Test1

const bool Bool = true;
// CHECK: Filename
// CHECK-SAME: BasicTests.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 17
// CHECK: Type
// CHECK-SAME: const _Bool
// CHECK: VarName
// CHECK-SAME: Bool

const unsigned Unsigned = 1U;
// CHECK: Filename
// CHECK-SAME: BasicTests.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 27
// CHECK: Type
// CHECK-SAME: const unsigned
// CHECK: VarName
// CHECK-SAME: Unsigned

const double Double = 1.0;
// CHECK: Filename
// CHECK-SAME: BasicTests.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 37
// CHECK: Type
// CHECK-SAME: const double
// CHECK: VarName
// CHECK-SAME: Double

const float Float = 1.0f;
// CHECK: Filename
// CHECK-SAME: BasicTests.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 47
// CHECK: Type
// CHECK-SAME: const float
// CHECK: VarName
// CHECK-SAME: Float

const long Long = 1000L;
// CHECK: Filename
// CHECK-SAME: BasicTests.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 57
// CHECK: Type
// CHECK-SAME: const long
// CHECK: VarName
// CHECK-SAME: Long

const unsigned long ULong = 1000UL;
// CHECK: Filename
// CHECK-SAME: BasicTests.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 67
// CHECK: Type
// CHECK-SAME: const unsigned long
// CHECK: VarName
// CHECK-SAME: ULong

const long long LLong = 1000LL;
// CHECK: Filename
// CHECK-SAME: BasicTests.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 77
// CHECK: Type
// CHECK-SAME: const long long
// CHECK: VarName
// CHECK-SAME: LLong

const unsigned long long ULLong = 1000ULL;
// CHECK: Filename
// CHECK-SAME: BasicTests.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 87
// CHECK: Type
// CHECK-SAME: const unsigned long long
// CHECK: VarName
// CHECK-SAME: ULLong

// CHECK: LocalConstVar:
// CHECK-SAME: []
// CHECK: ConstField:
// CHECK-SAME: []
// CHECK: TemplateLiteral:
// CHECK-SAME: []