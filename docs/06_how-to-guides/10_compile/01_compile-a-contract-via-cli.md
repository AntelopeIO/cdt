---
content_title: How to compile a smart contract via CLI
---

## Overview

This guide provides instructions how to compile a smart contract using the command line interface (CLI).

## Before you begin

* You have the source of the contract saved in a local folder, e.g. `./examples/hello/`
## Procedure

Follow the following steps to compile your contract.

1. Navigate to the hello folder in examples `./examples/hello`. You should see the `./src/hello.cpp` file.

2. Run the following commands:

    ```sh
    mkdir build
    cd build
    cdt-cpp ../src/hello.cpp -o hello.wasm -I ../include/
    ```

    Where:
    - `../src/hello.cpp` = Is the input cpp source file to be compiled.
    - `-o hello.wasm` = It instructs the `cdt--cpp` tool who to name the output wasm file.
    - `-I ../include/` = It tells `cdt-cpp` tool what the include folder path is, in this particular case it is a relative path.

3. Verify the following two files were generated:

* the compiled binary wasm: `hello.wasm`,
* and the generated ABI file: `hello.abi`.

## Summary

In conclusion, the above instructions show how to compile a smart contract using the command line interface (CLI).
