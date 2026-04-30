// Should test whether the UsedFunctions are correctly extracted if literals are used in binary operations
// Both for free functions and member functions

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s
// CHECK: GlobalConstVar:
// CHECK-SAME: []
// CHECK: LocalConstVar:
// CHECK-SAME: []
// CHECK: ConstField:
// CHECK-SAME: []

#include <cstdint>

// Free function that uses literals in binary operations
void freeFunctionUse() {
  int x = 2 + 4;
  (void)x;
}

// Class with a method that uses literals
struct U {
  void methodUse() {
    int y = 2 * 6;
    (void)y;
  }
};

// FileCheck expectations (single block at the end)
// CHECK: LiteralComp:
// CHECK: Filename
// CHECK-SAME: UsedFunctions.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 17
// CHECK: FunctionName
// CHECK-SAME: freeFunctionUse

// CHECK: Filename
// CHECK-SAME: UsedFunctions.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 24
// CHECK: FunctionName
// CHECK-SAME: U::methodUse
