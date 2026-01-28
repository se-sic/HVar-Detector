// LIT Test file for basic occurrences of binary operations where any operand is a literal
// Should include cases for most common binary operations
// E.g. arithmetic, comparisons, logical operations

// RUN: %hvf --report-file=%t --root-dir=%S %s
// RUN: cat %t | FileCheck %s
// CHECK: GlobalConstVar:
// CHECK-SAME: []
// CHECK: LocalConstVar:
// CHECK-SAME: []
// CHECK: ConstField:
// CHECK-SAME: []

#include <cstdint>
#include <cmath>

// Global arithmetic occurrences
int g_add = 1 + 2;
int g_sub = 10 - 3;
int g_mul = 4 * 5;
int g_div = 20 / 4;
int g_mod = 10 % 3;

// Bitwise and shift
int g_and = 0b1010 & 0xff;
int g_or = 0xF0 | 0x0F;
int g_xor = 0xAA ^ 0x55;
int g_shl = 1 << 4;
int g_shr = 0x80 >> 2;

// Comparisons
bool g_lt = 3 < 5;
bool g_gt = 10 > 2;
bool g_le = 4 <= 4;
bool g_ge = 6 >= 7;
bool g_eq = (5 == 5);
bool g_ne = (5 != 6);

// Logical
bool g_land = (1 && 0);
bool g_lor = (0 || 1);

// Floating point
float g_fmul = 1.5f * 2.0f;
double g_fadd = 3.14 + 2.0;

// Mixed signed/unsigned literal
unsigned g_mix = 5u + 3;

// Use inside a function
void useInFunction() {
  int local_calc = 7 * 8;
  (void)local_calc;
}

// FileCheck expectations (kept in a single block to avoid line offset issues)
// CHECK: LiteralComp:
// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 18
// CHECK: Operator
// CHECK-SAME: +

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 19
// CHECK: Operator
// CHECK-SAME: -

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 20
// CHECK: Operator
// CHECK-SAME: *

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 21
// CHECK: Operator
// CHECK-SAME: /

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 22
// CHECK: Operator
// CHECK-SAME: %

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 25
// CHECK: Operator
// CHECK-SAME: &

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 26
// CHECK: Operator
// CHECK-SAME: |

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 27
// CHECK: Operator
// CHECK-SAME: ^

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 28
// CHECK: Operator
// CHECK-SAME: <<

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 29
// CHECK: Operator
// CHECK-SAME: >>

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 32
// CHECK: Operator
// CHECK-SAME: <

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 33
// CHECK: Operator
// CHECK-SAME: >

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 34
// CHECK: Operator
// CHECK-SAME: <=

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 35
// CHECK: Operator
// CHECK-SAME: >=

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 36
// CHECK: Operator
// CHECK-SAME: ==

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 37
// CHECK: Operator
// CHECK-SAME: !=

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 40
// CHECK: Operator
// CHECK-SAME: &&

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 41
// CHECK: Operator
// CHECK-SAME: ||

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 44
// CHECK: Operator
// CHECK-SAME: *

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 45
// CHECK: Operator
// CHECK-SAME: +

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 48
// CHECK: Operator
// CHECK-SAME: +

// CHECK: Filename
// CHECK-SAME: BasicOccurences.cpp
// CHECK-NEXT: Lineno
// CHECK-SAME: 52
// CHECK: Operator
// CHECK-SAME: *

