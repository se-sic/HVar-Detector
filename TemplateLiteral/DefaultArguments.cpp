// Test whether default arguments in template literals are detected

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s
// CHECK: GlobalConstVar:
// CHECK-SAME: []
// CHECK: LocalConstVar:
// CHECK-SAME: []
// CHECK: ConstField:
// CHECK-SAME: []

#include "Common.h"

// Explicit argument instantiation
DefaultArgTemplate<5> explicitInst;
// CHECK: TemplateLiteral:
// CHECK: Filename
// CHECK-SAME: DefaultArguments.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 15
// CHECK: TemplateClassName
// CHECK-SAME: DefaultArgTemplate

// Default argument instantiation (uses the default N = 3)
DefaultArgTemplate<> defaultInst;
// Default arguments not matched, therefore check for end of file
// CHECK: ...