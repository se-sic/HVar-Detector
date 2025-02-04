// Test some very basic occurences of global const vars

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s

const int Test1 = 1000;
// CHECK: GlobalConstVar:
// CHECK: Filename
// CHECK-SAME: BasicTests.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 6
// CHECK: VarName
// CHECK-SAME: Test1

// CHECK: LocalConstVar:
// CHECK-SAME: []
// CHECK: ConstField:
// CHECK-SAME: []
// CHECK: TemplateLiteral:
// CHECK-SAME: []