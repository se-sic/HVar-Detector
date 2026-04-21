// Simple test file where no variable declaration should match
// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s
int foo() {
  const int i = 0;
  return i;
}

int I = 0;

const int I2 = foo();
static int I3 = 0;

// CHECK: GlobalConstVar
// CHECK-SAME: []