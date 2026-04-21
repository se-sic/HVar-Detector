// Test that matches in included files are also detected correctly

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s

#include "IncludedMatches.h"

// CHECK: GlobalConstVar
// CHECK: Filename
// CHECK-SAME: IncludedMatches.h
// CHECK-NEXT: Lineno
// CHECK-SAME: 4
// CHECK: Type
// CHECK-SAME: const int
// CHECK: VarName
// CHECK-SAME: HeaderInt
