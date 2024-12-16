---
title: CDT-LD
section: 1
header: Contract Development Toolkit (CDT)
footer: AntelopeIO
date: April 14, 2023
---
# NAME
cdt-ld - Antelope smart contract WebAssembly linker

# SYNOPSIS

`cdt-ld [options] <input file>`

# DESCRIPTION

**cdt-ld**  Is a compiled smart contract linker which creates WebAssemply code for 
execution in Antelope block chain virtual machines.  

# OPTIONS

**`-L=<string>`**

    Add directory to library search path

**`--abi-version=<string>`**

    Which ABI version to generate
    
**`--allow-names`**

    Allows name section to be created
    
**`--fasm`**

    Assemble file for x86-64
    
**`--fnative`**

    Compile and link for x86-64
    
**`--fcfl-aa`**

    Enable CFL Alias Analysis
    
**`--fno-lto`**

    Disable LTO

**`--fno-post-pass`**

    Don't run post processing pass
    
**`--fno-stack-first`**

    Don't set the stack first in memory
    
**`--fquery`**

    Produce binaries for wasmql
    
**`--fquery-client`**

    Produce binaries for wasmql
    
**`--fquery-server`**

    Produce binaries for wasmql
    
**`--fuse-main`**

    Use main as entry
    
**`--help`**

    Display available options (--help-hidden for more)
    
**`-l=<string>`**

    Root name of library to link
    
**`--lto-opt=<string>`**

    LTO Optimization level (O0-O3)

**`--mllvm=<string>`**

    Pass arguments to llvm
    
**`--no-abigen`**

    Disable ABI file generation
    
**`--no-missing-ricardian-clause`**

    Disable warnings for missing Ricardian clauses
    
**`-o=<string>`**

    Write output to <file>
    
**`--shared`**

    Make shared object native library

**`--stack-size=<int>`**

    Specifies the maximum stack size for the contract. Defaults to 8192 bytes
    
**`--version`**

    Display the version of this program

# BUGS

Please submit bug reports online at https://github.com/AntelopeIO/cdt/issues

# SEE ALSO

For more details consult the full documentation and sources https://github.com/AntelopeIO/cdt


