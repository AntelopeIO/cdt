---
content_title: EOSIO.CDT To Mandel.CDT
---

For this release all of the naming of namespaces and file paths will remain as `eosio`, so no need to migrate away from that with this release.

But there are a few differences.

1. The tool names have change prefix. I.e. tools like `eosio-cpp` are now `cdt-cpp`.
2. The cmake toolchain file has changed its name from `EosioWasmToolchain.cmake` to `CDTWasmToolchain.cmake`.
3. The library path for the cmake package and package name have changed. `/usr/local/lib/cmake/eosio.cdt` to `/usr/local/lib/cmake/cdt` and in cmake `find_package(eosio.cdt)` to `find_package(cdt)`.