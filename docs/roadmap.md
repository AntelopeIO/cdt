# Roadmap for CDT

## Summary Milestone 5
Top priority is clean-up of the code and optimizing for support. Target Date for Milestone 5 is May/June 2024.
- Remove AntlerProj repos from build and archive AntlerProj
- Move to C++20 - allow contracts to compile C++20
- Vanilla Clang/LLVM
- Upgrade to LLVM


## Antler
We lack the resources to complete the Antler project and support it going forward. Removing dependencies will simplify the build process and simplify things.

## Move to C++20
Allow contracts to compile C++20. In addition to the benefits from the latest language features. Currently, ENF maintains an additional fork of one of our upstream dependencies for EOS EVM simply to get around the fact that the upstream assumes C++20 but we cannot build C++20 code in our contracts.

## Vanilla Clang/LLVM
Try eosio extension free llvm and if it works with no issues then remove extensions to Clang/LLVM. This hope we will enable us to use Vanilla versions of the packages. This will allow us to use the latest, and will lead to improvements in functionality and performance.

## Upgrade to LLVM 16
Upgrade to the latest
