#!/bin/bash

set -x

INPUT_FILE=${1:-simple-main.wasm}

ROOT=/Users/eric/eosnetworkfoundation/repos/antelope/cdt

if "${ROOT}"/cmake-build-debug/tools/external/wabt/wasm-interp simple-main.wasm ; then
  echo "Wasm check of ${INPUT_FILE} OK"
else
  echo "FAILED wasm check of ${INPUT_FILE}"
  exit 127
fi

"${ROOT}"/cmake-build-debug/tools/external/wabt/wasm-interp simple-main.wasm --run-all-exports --trace

