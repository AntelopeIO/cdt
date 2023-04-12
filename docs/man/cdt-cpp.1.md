---
title: CDT-CPP
section: 1
header: User Manual
footer: cdt-cpp
date: April 08, 2023
---
# NAME
cdt-cpp - Eosio C++ to WebAssembly compiler

# SYNOPSIS

`cdt-cpp [options] <input file>`

# DESCRIPTION

**cdt-cpp**  Is a C++ compiler which converts C++ code of the smart contracts to WebAssemply binary code for 
execution in a virtual machine of Antelope block chains  

# OPTIONS

**`-C`** 

    Include comments in preprocessed output
    
**`--CC`** 

    Include comments from within macros in preprocessed output
    
**`-D=<name definition>`** 

    Define <macro> to <value> (or 1 if <value> omitted)

**`-U=<name>`** 

    Cancel any previous definition of name, either built in or provided with a -D option.              
    
**`-E`**

    Only run the preprocessor    
    
**`-I=<dir>`**
    
    Add directory to include search path
    
**`-L=<dir>`**

    Add directory to library search path    
    
**`--MF=<file>`**

    Write depfile output    

**`--MD`**

    Write depfile containing user and system headers    
    
**`--MMD`**
    
    Like -MD except mention only user header files, not system header files. 
    
**`--MT=<target>`**

    Specify name of main file output in depfile    
    
**`-O=<optimization_level>`**
    
    Optimization level s, 0-3     
    
**`-R=<dir>`**
    
    Add the directory dir to the list of directories to be searched for resource files.
    
**`-S`**

    Only run preprocess and compilation steps    
    
**`-W=<warning_name>`**
    
    Enable the specified warning.   
    
**`--abi-version=<string>`**
    
    Which ABI version to generate    
    
**`--abigen`**
    
    Generate ABI
    
**`--abigen_output=<string>`**
    
    ABIGEN output
    
**`--allow-names`**
    
    Allows name section to be created
    
**`--allow-sse`**
    
    Should not be used, except for build libc
    
**`--as-secure-log-file-name=<value>`**
    
    As secure log file name (initialized from AS_SECURE_LOG_FILE env variable)
    
**`--asm-macro-max-nesting-depth=<uint>`**
    
    The maximum nesting depth allowed for assembly macros.
    
**`-c`**
    
    Only run preprocess, compile, and assemble steps
    
**`--color`**
    
    Use colors in output (default=autodetect)
    
**`--contract=<string>`**
    
    Contract name
    
**`--dD`**
    
    Print macro definitions in -E mode in addition to normal output
    
**`--dI`**
    
    Print include directives in -E mode in addition to normal output
    
**`--dM`**
    
    Print macro definitions in -E mode instead to normal output
    
**`--disable-symbolication`**
    
    Disable symbolizing crash backtraces.
    
**`--emit-ast`**
    
    Emit Clang AST files for source inputs
    
**`--emit-ir`**
    
    Emit llvm ir
    
**`--emit-llvm`**
    
    Use the LLVM representation for assembler and object files
    
**`--eosio-pp-dir=<string>`**
    
    Set the directory for eosio-pp
    
**`--fPIC`**
    
    Generate position independent code. This option is used for shared libraries
    
**`--faligned-allocation`**
    
    Enable C++17 aligned allocation functions
    
**`--fasm`**
    
    Assemble file for x86-64
    
**`--fcolor-diagnostics`**
    
    Use colors in diagnostics
    
**`--fcoroutine-ts`**
    
    Enable support for the C++ Coroutines TS
    
**`--finline-functions`**
    
    Inline suitable functions
    
**`--finline-hint-functions`**
    
    Inline functions which are (explicitly or implicitly) marked inline
    
**`--fmerge-all-constants`**
    
    Allow merging of constants
    
**`--fnative`**
    
    Compile and link for x86-64
    
**`--fno-cfl-aa`**
    
    Disable CFL Alias Analysis
    
**`--fno-elide-constructors`**
    
    Disable C++ copy constructor elision
    
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
    
**`--fstack-protector`**
    
    Enable stack protectors for functions potentially vulnerable to stack smashing
    
**`--fstack-protector-all`**
    
    Force the usage of stack protectors for all functions
    
**`--fstack-protector-strong`**
    
    Use a strong heuristic to apply stack protectors to functions
    
**`--fstrict-enums`**
    
    Enable optimizations based on the strict definition of an enum's value range
    
**`--fstrict-return`**
    
    Always treat control flow paths that fall off the end of a non-void function as unreachable
    
**`--fstrict-vtable-pointers`**
    
    Enable optimizations based on the strict rules for overwriting polymorphic C++ objects
    
**`--fuse-main`**
    
    Use main as entry
        
**`-h`**
    
    Alias for --help
    
**`--help`**
    
    Display available options (--help-hidden for more)
    
**`--help-hidden`**
    
    Display all available options
    
**`--help-list`**
    
    Display list of available options (--help-list-hidden for more)
    
**`--help-list-hidden`**
    
    Display list of all available options
    
**`--imports=<string>`**
    
    Set the file for cdt.imports
    
**`--include=<string>`**
    
    Include file before parsing
    
**`--info-output-file=<filename>`**
    
    File to append -stats and -timer output to
    
**`--isysroot=<string>`**
    
    Set the system root directory (usually /)
    
**`--isystem=<string>`**
    
    Add directory to SYSTEM include search path
    
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
    
**`--non-global-value-max-name-size=<uint>`**
    
    Maximum size for the name of non-global values.
    
**`-o=<file>`**
    
    Write output to <file>
    
**`--only-export=<string>`**
    
    Export only this symbol
    
**`--opt-bisect-limit=<int>`**
    
    Maximum optimization to perform
    
**`--pass-remarks=<pattern>`**
    
    Enable optimization remarks from passes whose name match the given regular expression
    
**`--pass-remarks-analysis=<pattern>`**
    
    Enable optimization analysis remarks from passes whose name match the given regular expression
    
**`--pass-remarks-missed=<pattern>`**
    
    Enable missed optimization remarks from passes whose name match the given regular expression
    
**`--print-all-options`**
    
    Print all option values after command line parsing
    
**`--print-options`**
    
    Print non-default options after command line parsing
    
**`--remarks-yaml-string-table`**
    
    Enable the usage of a string table with YAML remarks.
    
**`--rng-seed=<seed>`**
    
    Seed for the random number generator
    
**`--shared`**
    
    Make shared object native library
    
**`--stack-canary`**
    
    Stack canary for non stack first layouts
    
**`--stack-size=<int>`**
    
    Specifies the maximum stack size for the contract. Defaults to 8192 bytes.
    
**`--static-func-full-module-prefix`**
    
    Use full module build paths in the profile counter names for static functions.
    
**`--static-func-strip-dirname-prefix=<uint>`**
    
    Strip specified level of directory name from source path in the profile counter name for static functions.
    
**`--stats`**
    
    Enable statistics output from program (available with Asserts)
    
**`--stats-json`**
    
    Display statistics as json data
    
**`--std=<string>`**
    
    Language standard to compile for
    
**`--switch-inst-prof-update-wrapper-strict`**
    
    Assert that prof branch_weights metadata is valid when creating an instance of SwitchInstProfUpdateWrapper
    
**`--sysroot=<string>`**
    
    Set the system root directory
    
**`--time-trace-granularity=<uint>`**
    
    Minimum time granularity (in microseconds) traced by time profiler
    
**`--track-memory`**
    
    Enable -time-passes memory tracking (this may be slow)
    
**`--use-dbg-addr`**
    
    Use llvm.dbg.addr for all local variables
    
**`--use-freeing-malloc`**
    
    Set the malloc implementation to the old freeing malloc
    
**`--use-rt`**
    
    Use software compiler-rt
    
**`-v`**
    
    Show commands to run and use verbose output
    
**`--version`**
    
    Display the version of this program
    
**`--view-background`**
    
    Execute graph viewer in the background. Creates tmp file litter.
    
**`-w`**
    
    Suppress all warnings
    
**`--warn-action-read-only`**
    
    Issue a warning if a read-only action uses a write API and continue compilation
    

# BUGS

Please submit bug reports online at https://github.com/AntelopeIO/cdt/issues

# SEE ALSO

For more details consult the full documentation and sources https://github.com/AntelopeIO/cdt


