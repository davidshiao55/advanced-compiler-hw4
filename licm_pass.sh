#!/bin/bash

# Check if a test file is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <test_file.c>"
    exit 1
fi

TEST_FILE=$1
BASENAME=$(basename "$TEST_FILE" .c)

# Ensure the test file exists
if [ ! -f "$TEST_FILE" ]; then
    echo "Error: File $TEST_FILE not found!"
    exit 1
fi

# Compile the test file to LLVM IR
clang -Xclang -disable-O0-optnone -O0 -emit-llvm -S "$TEST_FILE" -o "${BASENAME}.ll"

# Run the mem2reg pass
opt -S -passes=mem2reg "${BASENAME}.ll" -o "${BASENAME}_mem2reg.ll"

# Compile the processed IR to an executable
clang -O0 "${BASENAME}_mem2reg.ll" -o "${BASENAME}_exec"

# Run the custom LICM pass
opt -load-pass-plugin=./MySimpleLICMPass.so -passes="loop(my-simple-licm-pass)" \
    -S < "${BASENAME}_mem2reg.ll" > "${BASENAME}_licm.ll"

# Compile the LICM processed IR to an executable
clang -O0 "${BASENAME}_licm.ll" -o "${BASENAME}_licm_exec"

# Run the baseline and LICM versions
echo "BASELINE"
./"${BASENAME}_exec"
echo "LICM"
./"${BASENAME}_licm_exec"

# Clean up intermediate files
rm "${BASENAME}.ll" "${BASENAME}_mem2reg.ll" "${BASENAME}_licm.ll" \
   "${BASENAME}_exec" "${BASENAME}_licm_exec"
