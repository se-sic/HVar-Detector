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
  // CHECK: Filename
  // CHECK-SAME: FunctionUses.cpp
  // CHECK-NEXT: Lineno
  // CHECK-SAME: 16
  // CHECK: TemplateClassName
  // CHECK-SAME: foo
}

// Member function template instantiation on an object
void memberFuncUse() {
  C<1> obj;
  obj.memberFunc<10>();
  // CHECK: Filename
  // CHECK-SAME: FunctionUses.cpp
  // CHECK-NEXT: Lineno
  // CHECK-SAME: 29
  // CHECK: TemplateClassName
  // CHECK-SAME: memberFunc
}

// Static member function template instantiation
void staticMemberFuncUse() {
  C<2>::staticMemberFunc<20>();
  // CHECK: Filename
  // CHECK-SAME: FunctionUses.cpp
  // CHECK-NEXT: Lineno
  // CHECK-SAME: 40
  // CHECK: TemplateClassName
  // CHECK-SAME: staticMemberFunc
}

// Static data member usage (value is non-type dependent)
void staticDataMemberUse() {
  int v = C<3>::value;
  // CHECK: Filename
  // CHECK-SAME: FunctionUses.cpp
  // CHECK-NEXT: Lineno
  // CHECK-SAME: 51
  // CHECK: TemplateClassName
  // CHECK-SAME: C
}
