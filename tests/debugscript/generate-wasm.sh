#!/bin/bash

set -x

INPUT_FILE=${1:-simple-main.cpp}
SOURCE=${INPUT_FILE%.*}
ROOT=/Users/eric/eosnetworkfoundation/repos/antelope/cdt

if [ ! -s "${INPUT_FILE}" ]; then
  echo "Could not file ${INPUT_FILE} file; exiting "
  exit 127
fi

BUILD_DIR="build"
if [ ! -d "${ROOT}"/${BUILD_DIR} ] && [ -d"${ROOT}"/cmake-build-debug ]; then
  BUILD_DIR=cmake-build-debug
fi

SYSTEM_INCLUDE=/usr/include
SYSTEM_ARCH_FLAG="__LINUX__"
if [ $(uname -o) == "Darwin" ]; then
  SYSTEM_INCLUDE=$(xcrun --show-sdk-path)/usr/include
  SYSTEM_ARCH_FLAG="__APPLE__"
fi
ARCH=$(uname -m)
CDT_VERSION="-D__eosio_cdt__ -D__eosio_cdt_major__=5 -D__eosio_cdt_minor__=0 -D__eosio_cdt_patchlevel__=0"

# W Args
# WARGS="-Wall -Werror-implicit-function-declaration -Wno-conversion -Wall -Wno-everything"
# softfloat
# -I"${ROOT}"/${BUILD_DIR}/libraries/native/softfloat/source/include
# -I"${ROOT}"/${BUILD_DIR}/libraries/native/softfloat/source/8086-SSE
# -I"${ROOT}"/${BUILD_DIR}/libraries/native/softfloat/build/Linux-x86_64-GCC \
# libraries
# -I"${ROOT}"/${BUILD_DIR}/libraries/eosiolib/capi
# -I"${ROOT}"/${BUILD_DIR}/libraries/eosiolib/contracts
# -I"${ROOT}"/${BUILD_DIR}/libraries/eosiolib/core \

[ -f "${SOURCE}".o ] && rm "${SOURCE}".o

${ROOT}/${BUILD_DIR}/bin/clang-16 \
"${CDT_VERSION}" \
-Wunused-command-line-argument \
--target=x86_64-unknown-darwin-macho \
-mmacosx-version-min=10.13 \
-fno-stack-protector -m64 -fno-builtin \
-mstackrealign -D__eosio_cdt_native__ \
-U${SYSTEM_ARCH_FLAG} \
-DEOSIO_NATIVE -DLLP64 -fno-threadsafe-statics \
-I"${ROOT}"/${BUILD_DIR}/include/libcxx -I"${ROOT}"/${BUILD_DIR}/include/libc \
-I"${ROOT}"/${BUILD_DIR}/include \
--sysroot="${ROOT}"/${BUILD_DIR} \
-I"${ROOT}"/${BUILD_DIR}/include/eosiolib/native/ -I"${ROOT}"/${BUILD_DIR}/include/eosiolib/capi \
-I"${ROOT}"/${BUILD_DIR}/include/eosiolib/core -I"${ROOT}"/${BUILD_DIR}/include/eosiolib/contracts \
-o "${SOURCE}".o -c "${INPUT_FILE}"

#--gc-sections --strip-all --no-merge-data-segments
# --entry apply --export=apply:function --export-memory
#--stack-first
#--lto-O3
#-zstack-size=8192

if [ -f "${SOURCE}".o ]; then
  ${ROOT}/${BUILD_DIR}/bin/wasm-ld \
      -lc++ -lc -leosio -leosio_dsm \
      -L"${ROOT}"/${BUILD_DIR}/lib \
      -o "${SOURCE}".o --no-entry --allow-undefined -o "${SOURCE}".wasm
else
  echo "No ${SOURCE}.wasm file no running wasm-ld"
fi
