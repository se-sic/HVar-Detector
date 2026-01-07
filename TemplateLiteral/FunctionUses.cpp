// Test file to check whether instantiations of templated functions are detected

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s
// CHECK: GlobalConstVar:
// CHECK-SAME: []
// CHECK: LocalConstVar:
// CHECK-SAME: []
// CHECK: ConstField:
// CHECK-SAME: []

#include "Common.h"

// Free function template instantiation
void freeFuncUse() {
  foo<5>();
  // CHECK: TemplateLiteral:
  // XCHECK: Filename
  // XCHECK-SAME: FunctionUses.cpp
  // XCHECK-NEXT: Lineno
  // XCHECK-SAME: 16
  // XCHECK: TemplateClassName
  // XCHECK-SAME: foo
}

// Member function template instantiation on an object
void memberFuncUse() {
  C<1> obj;
  obj.memberFunc<10>();
  // CHECK: Filename
  // CHECK-SAME: FunctionUses.cpp
  // CHECK-NEXT: Lineno
  // CHECK-SAME: 28
  // CHECK: TemplateClassName
  // CHECK-SAME: C
  // Missing: Match for line 29 due to current limitation in HVF
}

// Static member function template instantiation
void staticMemberFuncUse() {
  C<2>::staticMemberFunc<20>();
  // CHECK: Filename
  // CHECK-SAME: FunctionUses.cpp
  // CHECK-NEXT: Lineno
  // CHECK-SAME: 41
  // CHECK: TemplateClassName
  // CHECK-SAME: C
  // Missing: Match for line 42 due to current limitation in HVF
}

// Static data member usage (value is non-type dependent)
void staticDataMemberUse() {
  int v = C<3>::value;
  // CHECK: Filename
  // CHECK-SAME: FunctionUses.cpp
  // CHECK-NEXT: Lineno
  // CHECK-SAME: 53
  // CHECK: TemplateClassName
  // CHECK-SAME: C
}
