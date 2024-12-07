#!/bin/bash

# A script to prove llvm compiles on its own.
# usefull if you have having problems and need to compare generate build files

set -x

PWD=$(pwd)
build_dir=${PWD}/build-llvm-16.0.6
source_dir=${PWD}/llvm/
build_cache=OFF
installprefix=${PWD}/install

CLEAN=${1:-NO}
if [ $CLEAN = "YES" ]; then
  rm -rf $build_dir
  mkdir -p ${build_dir}/llvm ${build_dir}/clang
fi

[! -d ${build_dir}/llvm ] && mkdir -p ${build_dir}/llvm
[! -d ${installprefix} ] && mkdir -p ${installprefix}

cmake -S ${source_dir}/llvm -B ${build_dir}/llvm -G 'Unix Makefiles' \
-DCMAKE_INSTALL_PREFIX=$installprefix  \
-DCMAKE_BUILD_TYPE=Release \
-DLLVM_ENABLE_PROJECTS=clang;lld \
-DLLVM_CCACHE_BUILD=${build_cache} \

cd ${build_dir}/llvm && make -j 8
make install
cd $PWD
