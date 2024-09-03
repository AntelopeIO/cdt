# CDT (Contract Development Toolkit)

Contract Development Toolkit (CDT) is a C/C++ toolchain targeting WebAssembly (WASM) and a set of tools to facilitate development of smart contracts written in C/C++ that are meant to be deployed to an [Antelope](https://github.com/AntelopeIO/) blockchain.

In addition to being a general purpose WebAssembly toolchain, specific features and optimizations are available to support building Antelope-based smart contracts. This new toolchain is built around [Clang 9](https://github.com/AntelopeIO/cdt-llvm), which means that CDT inherits the optimizations and analyses from that version of LLVM, but as the WASM target is still considered experimental, some optimizations are incomplete or not available.

## Repo organization

The `main` branch is the development branch: do not use this for production. Refer to the [release page](https://github.com/AntelopeIO/cdt/releases) for current information on releases, pre-releases, and obsolete releases as well as the corresponding tags for those releases.
## Binary packages

CDT currently supports Linux x86_64 Debian packages. Visit the [release page](https://github.com/AntelopeIO/cdt/releases) to download the package for the appropriate version of CDT. This is the fastest way to get started with the software.
### Debian package install

Download the appropriate version of the Debian package and then install it. To download and install the latest version, run the following:

```sh
wget https://github.com/AntelopeIO/cdt/releases/download/v4.1.0/cdt_4.1.0_amd64.deb
sudo apt install ./cdt_4.1.0_amd64.deb
```
### Debian package uninstall

To remove CDT that was installed using a Debian package, simply execute the following command:

```sh
sudo apt remove cdt
```

## Building from source

Recent Ubuntu LTS releases are the only Linux distributions that we fully support. Other Linux distros and other POSIX operating systems (such as macOS) are tended to on a best-effort basis and may not be full featured.

The instructions below assume that you are building on Ubuntu 20.04.

### Install dependencies

```sh
apt-get update && apt-get install   \
        build-essential             \
        clang                       \
        clang-tidy                  \
        cmake                       \
        git                         \
        libxml2-dev                 \
        opam ocaml-interp           \
        python3                     \
        python3-pip                 \
        time
```

```sh
python3 -m pip install pygments
```

### Allowing integration tests to build

Integration tests require access to a build of [Spring](https://github.com/AntelopeIO/spring), a C++ implementation of the Antelope protocol. Simply installing Spring from a binary package will not be sufficient.

If you do not wish to build Spring, you can continue with building CDT but without building the integration tests. Otherwise, follow the instructions below before running `cmake`.

First, ensure that Spring has been built from source (see Spring's [README](https://github.com/AntelopeIO/spring#building-from-source) for details) and identify the build path, e.g. `/path/to/spring/build/`.

Then, execute the following command in the same terminal session that you will use to build CDT:

```sh
export spring_DIR=/path/to/spring/build/lib/cmake/spring
```

Now you can continue with the steps to build CDT as described. When you run `cmake` make sure that it does not report `spring package not found`. If it does, this means CDT was not able to find a build of Spring at the specified path in `spring_DIR` and will therefore continue without building the integration tests.

### ccache

If issues persist with ccache when building CDT, you can disable ccache:

```sh
export CCACHE_DISABLE=1
```

### Build CDT

**A Warning On Parallel Compilation Jobs (`-j` flag)**: When building C/C++ software often the build is performed in parallel via a command such as `make -j $(nproc)` which uses the number of CPU cores as the number of compilation jobs to perform simultaneously. However, be aware that some compilation units (.cpp files) in CDT are extremely complex and can consume a large amount of memory to compile. If you are running into issues due to amount of memory available on your build host, you may need to reduce the level of parallelization used for the build. For example, instead of `make -j $(nproc)` you can try `make -j2`. Failures due to memory exhaustion will typically but not always manifest as compiler crashes.

```sh
git clone --recursive https://github.com/AntelopeIO/cdt
cd cdt
mkdir build
cd build
cmake ..
make -j $(nproc)
```

The binaries will be located at in the `build/bin` directory. You can export the path to the directory to your `PATH` environment variable which allows you to conveniently use them to compile contracts without installing CDT globally. Alternatively, you can use CMake toolchain file located in `build/lib/cmake/CDTWasmToolchain.cmake` to compile the contracts in your CMake project, which also allows you to avoid installing CDT globally.

If you would prefer to install CDT globally, see the section [Install CDT](#install-cdt) below.

#### Build CDT in debug mode

To build CDT in debug mode (with debug symbols) you need to add the following flags to cmake command:
```sh
cmake -DCMAKE_BUILD_TYPE="Debug" -DTOOLS_BUILD_TYPE="Debug" -DLIBS_BUILD_TYPE="Debug" ..
```

### Run tests

#### Run unit tests

```sh
cd build

ctest
```

#### Run integration tests (if built)

```sh
cd build/tests/integration

ctest
```

### Install CDT

Installing CDT globally on your system will install the following tools in a location accessible to your `PATH`:

* cdt-abidiff
* cdt-ar
* cdt-cc
* cdt-cpp
* cdt-init
* cdt-ld
* cdt-nm
* cdt-objcopy
* cdt-objdump
* cdt-ranlib
* cdt-readelf
* cdt-strip
* eosio-pp
* eosio-wasm2wast
* eosio-wast2wasm

It will also install CMake files for CDT accessible within a `cmake/cdt` directory located within your system's `lib` directory.
#### Manual installation

One option for installing CDT globally is via `make install`. From within the `build` directory, run the following command:

```
sudo make install
```

#### Package installation

A better option for installing CDT globally is to generate a package and then install the package. This makes uninstalling CDT much easier.

From within the `build` directory, run the following commands to generate a Debian package:

```sh
cd packages
bash ./generate_package.sh deb ubuntu-20.04 amd64
sudo apt install ./cdt_*_amd64.deb
```

### Uninstall CDT

#### Uninstall CDT after manual installation with make

```sh
sudo rm -fr /usr/local/cdt
sudo rm -fr /usr/local/lib/cmake/cdt
sudo rm /usr/local/bin/eosio-*
sudo rm /usr/local/bin/cdt-*
```

#### Uninstall CDT that was installed using a Debian package

```sh
sudo apt remove cdt
```

## License

[MIT](./LICENSE)
