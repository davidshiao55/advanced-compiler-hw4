# README

## Environment Setup

### Required LLVM Version
- LLVM Version: **18.1.8**

### Dependencies
- `clang++`: Compatible with the LLVM version installed.
- `clang`: Compatible with the LLVM version installed.
- `opt`: Included in the LLVM suite for running passes.

---

## Build Instructions

### Compilation Commands
- To build the optimization pass:
  ```bash
  ./compile_liscm.sh
  ```
  This script compiles the `MySimpleLICMPass.cpp` file into a shared object file `MySimpleLICMPass.so`.

---

## Running the Pass

### Command to Run
To run the LICM pass on a test file:
```bash
./licm_pass.sh <test_file.c>
```

### Description of Command-Line Options
- `<test_file.c>`: The C source file to test.

The script performs the following steps:
1. Compiles the input C file to LLVM IR.
2. Applies the `mem2reg` optimization pass.
3. Applies the custom LICM pass (`loop(my-simple-licm-pass)`).
4. Compiles the transformed IR into an executable.
5. Compares the output of the baseline and LICM-transformed binaries.

## Example
```bash
$ ./compile_liscm.sh
$ ./licm_pass.sh tests/basic.c 
[MySimpleLICM] Hoisted:   %2 = mul nsw i32 5, 7
BASELINE
Result: 845000000
Time elapsed: 0.089 s
LICM
Result: 845000000
Time elapsed: 0.068 s
```