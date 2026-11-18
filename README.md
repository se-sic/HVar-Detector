# HVar-Detector

`hidden-variability-finder` is a Clang-based tool that scans C/C++ sources for
hidden variability candidates (constant variables and fields, literal template
arguments, literals in binary operations) and writes them to a YAML report.

## Requirements

- CMake >= 3.13 and a build tool (Ninja or Make)
- A C++20 compiler
- LLVM and Clang development files (CMake configs, headers, libraries)
- Only for running the tests: `lit` and `FileCheck`

On Debian/Ubuntu (tested with LLVM 20 on Debian 13):

```sh
sudo apt install cmake ninja-build g++ llvm-20-dev libclang-20-dev clang-20
sudo apt install llvm-20-tools   # only for the tests
```

## Build

```sh
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_DIR=/usr/lib/llvm-20/cmake \
    -DClang_DIR=/usr/lib/cmake/clang-20
cmake --build build
```

The binary is written to `build/bin/hidden-variability-finder`.

`LLVM_DIR` and `Clang_DIR` can be omitted if only one LLVM version is
installed. With several versions installed, set both: otherwise CMake may mix
versions and the link step fails.

The project can also be built in-tree as part of VaRA; this is detected
automatically when `VARA_SOURCE_DIR` and `LLVM_MAIN_SRC_DIR` are defined.

## Tests

The test target is only created if `llvm-lit` and `FileCheck` are found at
configure time. They are looked up on your `PATH`; if they are not there (as
with the Debian packages), pass their locations explicitly:

```sh
cmake -B build \
    -DLLVM_LIT_EXECUTABLE=/usr/lib/llvm-20/build/utils/lit/lit.py \
    -DLLVM_FILECHECK_EXECUTABLE=/usr/lib/llvm-20/bin/FileCheck
cmake --build build --target check-hidden-var-finder
```

## Usage

```sh
build/bin/hidden-variability-finder \
    --report-file=report.yml --root-dir=<project-root> \
    -p <dir-with-compile_commands.json> <source-files...>
```

Without `--report-file`, the report is written to
`hidden-configurability-report.yml`. Run with `--help` for all options.
