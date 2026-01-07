// Tests detection in different contexts

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s
// CHECK: GlobalConstVar:
// CHECK-SAME: []
// CHECK: LocalConstVar:
// CHECK-SAME: []
// CHECK: ConstField:
// CHECK-SAME: []

#include "Common.h"

namespace test
{
    // Check whether detection in namespaces works.
    // Instantiate template in namespace context
    IntLiteralTemplate<7> ns_inst;
    // CHECK: TemplateLiteral:
    // CHECK: Filename
    // CHECK-SAME: Contexts.cpp
    // CHECK-NEXT: Lineno
    // CHECK-SAME: 18
    // CHECK: TemplateClassName
    // CHECK-SAME: IntLiteralTemplate
}

void testFunction() {
    // Function to test context detection.
    IntLiteralTemplate<7> func_inst;
    // CHECK: TemplateLiteral:
    // CHECK: Filename
    // CHECK-SAME: Contexts.cpp
    // CHECK-NEXT: Lineno
    // CHECK-SAME: 30
    // CHECK: TemplateClassName
    // CHECK-SAME: IntLiteralTemplate

}

class TestClass
{
    // Class to test context detection.
    // Add member instantiation
    IntLiteralTemplate<7> member_inst;
    // CHECK: TemplateLiteral:
    // CHECK: Filename
    // CHECK-SAME: Contexts.cpp
    // CHECK-NEXT: Lineno
    // CHECK-SAME: 45
    // CHECK: TemplateClassName
    // CHECK-SAME: IntLiteralTemplate

    // Add a use inside a method
public:
    void useInMethod() {
        IntLiteralTemplate<7> method_inst;
        // CHECK: TemplateLiteral:
        // CHECK: Filename
        // CHECK-SAME: Contexts.cpp
        // CHECK-NEXT: Lineno
        // CHECK-SAME: 57
        // CHECK: TemplateClassName
        // CHECK-SAME: IntLiteralTemplate
    }

};

template <typename T>
class TestTemplateClass
{
    // Template class to test context detection.
    // Add member instantiation inside template class
    IntLiteralTemplate<7> templ_inst;
};

// Instantiation necessary, otherwise the code is not generated and thus cannot be detected.
TestTemplateClass<void> global_templ_inst;
// CHECK: TemplateLiteral:
// CHECK: Filename
// CHECK-SAME: Contexts.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 74
// CHECK: TemplateClassName
// CHECK-SAME: IntLiteralTemplate