cmake_minimum_required(VERSION 3.5...4.0)

# Sanity check our source directory to make sure that we are not trying to
# generate an in-source build, and to make
# sure that we don't have any stray generated files lying around in the tree
if( CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed.
Please create a directory and run cmake from there, passing the path
to this source directory as the last argument.
This process created the file `CMakeCache.txt' and the directory `CMakeFiles'.
Please delete them.")
endif()

project(cdt)

set(VERSION_MAJOR 5)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION_SUFFIX "dev1")

if (VERSION_SUFFIX)
   set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-${VERSION_SUFFIX}")
else()
   set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
endif()

set(CDT_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/cdt)

option(ENABLE_NATIVE_COMPILER "enable native builds with the CDT toolchain" ON)
option(ENABLE_TESTS "enable building tests" ON)

include(GNUInstallDirs)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
endif()

if(NOT TOOLS_BUILD_TYPE)
   set(TOOLS_BUILD_TYPE "Release")
endif()

if(NOT LIBS_BUILD_TYPE)
   set(LIBS_BUILD_TYPE "Release")
endif()

include(modules/ClangExternalProject.txt)
include(modules/ToolsExternalProject.txt)

set(WASM_SDK_BUILD true)

### Configure the CDTWasmToolchain.cmakes
set(CDT_ROOT_DIR ${CMAKE_BINARY_DIR})

configure_file(${CMAKE_SOURCE_DIR}/modules/cdt-config.cmake ${CMAKE_BINARY_DIR}/lib/cmake/cdt/cdt-config.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/modules/CDTMacros.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/cdt/CDTMacros.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/modules/CDTWasmToolchain.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/cdt/CDTWasmToolchain.cmake @ONLY)

set(CDT_ROOT_DIR ${CDT_INSTALL_PREFIX})

configure_file(${CMAKE_SOURCE_DIR}/modules/cdt-config.cmake ${CMAKE_BINARY_DIR}/modules/cdt-config.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/modules/CDTMacros.cmake.in ${CMAKE_BINARY_DIR}/modules/CDTMacros.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/modules/CDTWasmToolchain.cmake.in ${CMAKE_BINARY_DIR}/modules/CDTWasmToolchain.cmake @ONLY)

set(CDT_ROOT_DIR "_PREFIX_")
configure_file(${CMAKE_SOURCE_DIR}/modules/CDTMacros.cmake.in ${CMAKE_BINARY_DIR}/modules/CDTMacrosPackage.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/modules/CDTWasmToolchain.cmake.in ${CMAKE_BINARY_DIR}/modules/CDTWasmToolchainPackage.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/modules/cdt-config.cmake ${CMAKE_BINARY_DIR}/modules/cdt-config.cmake.package @ONLY)

include(modules/LibrariesExternalProject.txt)

include(modules/InstallCDT.cmake)

configure_file(${CMAKE_SOURCE_DIR}/imports/cdt.imports.in ${CMAKE_BINARY_DIR}/cdt.imports COPYONLY)
install(FILES ${CMAKE_BINARY_DIR}/cdt.imports DESTINATION ${CDT_INSTALL_PREFIX})

configure_file(${CMAKE_SOURCE_DIR}/scripts/ricardeos/ricardeos.py ${CMAKE_BINARY_DIR}/scripts/ricardeos.py COPYONLY)
install(FILES ${CMAKE_BINARY_DIR}/scripts/ricardeos.py DESTINATION ${CDT_INSTALL_PREFIX}/scripts)

# section for package construction
set(VENDOR "EOSNetworkFoundation")
set(PROJECT_NAME "cdt")
set(DESC "Toolchain and supporting tools to facilitate C/C++ development of contracts for Antelope blockchains")
set(URL "https://github.com/AntelopeIO/cdt")
set(EMAIL "support@eosnetwork.com")
configure_file(${CMAKE_SOURCE_DIR}/scripts/generate_package.sh.in ${CMAKE_BINARY_DIR}/packages/generate_package.sh @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/scripts/generate_deb.sh ${CMAKE_BINARY_DIR}/packages/generate_deb.sh COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/scripts/generate_bottle.sh ${CMAKE_BINARY_DIR}/packages/generate_bottle.sh COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/scripts/generate_rpm.sh ${CMAKE_BINARY_DIR}/packages/generate_rpm.sh COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/scripts/generate_tarball.sh ${CMAKE_BINARY_DIR}/packages/generate_tarball.sh COPYONLY)

# add licenses
configure_file(${CMAKE_SOURCE_DIR}/cdt-llvm/LICENSE.TXT ${CMAKE_BINARY_DIR}/licenses/llvm.license COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/libraries/boost/boost.license ${CMAKE_BINARY_DIR}/licenses/boost.license COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/libraries/meta_refl/LICENSE ${CMAKE_BINARY_DIR}/licenses/meta_refl.license COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/tools/external/wabt/LICENSE ${CMAKE_BINARY_DIR}/licenses/wabt.license COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/tools/jsoncons/LICENSE ${CMAKE_BINARY_DIR}/licenses/jsoncons.license COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/LICENSE ${CMAKE_BINARY_DIR}/licenses/cdt.license COPYONLY)

include(modules/TestsExternalProject.txt)

include(CTest)
enable_testing()
add_subdirectory(tests)
