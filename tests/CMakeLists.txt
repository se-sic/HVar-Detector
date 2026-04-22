
if(NOT IN_VARA_BUILD)
# Special setup required to correctly setup llvm-lit and FileCheck
set(LLVM_LIT_EXECUTABLE "" CACHE STRING "Path to llvm-lit executable")
set(LLVM_FILECHECK_EXECUTABLE "" CACHE STRING "Path to FileCheck executable")

# In the build directory create symlinks to the provided executables
if(NOT LLVM_LIT_EXECUTABLE)
    # Try to find llvm-lit in the system path
    find_program(LLVM_LIT_EXECUTABLE llvm-lit)
    if(NOT LLVM_LIT_EXECUTABLE)
        message(WARNING "No llvm-lit executable provided and not found in system path. This might impact the test suite. Please set LLVM_LIT_EXECUTABLE to the path of llvm-lit.")
    else()
        message(STATUS "Found llvm-lit executable at ${LLVM_LIT_EXECUTABLE}")
    endif()
endif ()

# In the build directory create symlinks to the provided executables
if(NOT LLVM_FILECHECK_EXECUTABLE)
    # Try to find FileCheck in the system path
    find_program(LLVM_FILECHECK_EXECUTABLE FileCheck)
    if(NOT LLVM_FILECHECK_EXECUTABLE)
        message(WARNING "No FileCheck executable provided and not found in system path. This might impact the test suite. Please set LLVM_FILECHECK_EXECUTABLE to the path of FileCheck.")
    else()
        message(STATUS "Found FileCheck executable at ${LLVM_FILECHECK_EXECUTABLE}")
    endif()
endif ()

message(STATUS "Using FileCheck executable at ${LLVM_FILECHECK_EXECUTABLE}")
message(STATUS "Creating symlink to FileCheck in build directory at ${CMAKE_CURRENT_BINARY_DIR}/FileCheck")
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/FileCheck
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${LLVM_FILECHECK_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/FileCheck
)
add_custom_target(
        FileCheck-symlink ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/FileCheck
)

# Set LLVM_EXTERNAL_LIT to the found llvm-lit executable, so that it can be used in the test suite

# Check if llvm-lit path exists and is executable
if(EXISTS ${LLVM_LIT_EXECUTABLE})
    message(STATUS "llvm-lit executable is valid and executable at ${LLVM_LIT_EXECUTABLE}")
else()
    message(FATAL_ERROR "llvm-lit executable at ${LLVM_LIT_EXECUTABLE} is not valid or not executable. This might impact the test suite. Please set LLVM_LIT_EXECUTABLE to a valid and executable path.")
endif()

set(LLVM_EXTERNAL_LIT ${LLVM_LIT_EXECUTABLE})
message(STATUS "Using llvm-lit executable at ${LLVM_EXTERNAL_LIT}")

configure_lit_site_cfg(
        ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
        ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
        MAIN_CONFIG
        ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg
)
endif ()

add_lit_testsuite(
        check-hidden-var-finder "Running Hidden Variability regression tests"
        ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS FileCheck-symlink hidden-variability-finder
)