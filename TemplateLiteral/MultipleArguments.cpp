// Tests to check whether function and class template instantiations with multiple
// non-type arguments are detected when one or more arguments are literals

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s
// CHECK: GlobalConstVar:
// CHECK-SAME: []
// CHECK: LocalConstVar:
// CHECK-SAME: []
// CHECK: ConstField:
// CHECK-SAME: []

#include "Common.h"

// Instantiate class template with two integral non-type template args
MultiIntTemplate<1, 2> mi_inst;
// CHECK: TemplateLiteral:
// CHECK: Filename
// CHECK-SAME: MultipleArguments.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 16
// CHECK: TemplateClassName
// CHECK-SAME: MultiIntTemplate

// Instantiate mixed-type template with an integral and a char literal
MixedTemplate<7, 'a'> mixed_inst;
// CHECK: Filename
// CHECK-SAME: MultipleArguments.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 26
// CHECK: TemplateClassName
// CHECK-SAME: MixedTemplate

// Instantiate function template with two integral arguments
void useFnMulti() {
  fnMulti<3, 4>();
  // CHECK: Filename
  // CHECK-SAME: MultipleArguments.cpp
  // CHECK-NEXT: Lineno
  // CHECK-SAME: 36
  // CHECK: TemplateClassName
  // CHECK-SAME: fnMulti
}

// Instantiate class template mixing a type parameter and a non-type parameter
TypeAndNonType<int, 5> type_nontype_inst;
// CHECK: Filename
// CHECK-SAME: MultipleArguments.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 46
// CHECK: TemplateClassName
// CHECK-SAME: TypeAndNonType

// Instantiate parameter pack templates
PackIntTemplate<1, 2, 3> pack_int_inst;
// CHECK: Filename
// CHECK-SAME: MultipleArguments.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 55
// CHECK: TemplateClassName
// CHECK-SAME: PackIntTemplate

PackMixedTemplate<int, 4, 5> pack_mixed_inst;
// CHECK: Filename
// CHECK-SAME: MultipleArguments.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 63
// CHECK: TemplateClassName
// CHECK-SAME: PackMixedTemplate
