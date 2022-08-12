# CDT (Contract Development Toolkit)
## Version : 3.0.0

CDT is a toolchain for WebAssembly (WASM) and a set of tools to facilitate smart contract development for the Leap platform, a C++ implementation of the Antelope protocol. In addition to being a general purpose WebAssembly toolchain, [Leap](https://github.com/AntelopeIO/leap) specific optimizations are available to support building smart contracts.  This new toolchain is built around [Clang 9](https://github.com/AntelopeIO/cdt-llvm), which means that CDT has the most currently available optimizations and analyses from LLVM, but as the WASM target is still considered experimental, some optimizations are incomplete or not available.

## Binary Releases
CDT currently supports Linux x86_64 Debian packages.
### Debian Package Install
```sh
wget https://github.com/AntelopeIO/cdt/releases/download/v3.0.0-rc1/cdt_3.0.0-rc1_amd64.deb
sudo apt install ./cdt_3.0.0-rc1_amd64.deb
```
### Debian Package Uninstall
```sh
sudo apt remove cdt
```

## Building

### Ubuntu 20.04 dependencies
```sh
apt-get update && apt-get install   \
        build-essential             \
        clang                       \
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

If issues persist with ccache
```sh
export CCACHE_DISABLE=1
```

### Building Integration Tests

Integration tests require access to a Leap build.  Instructions below provide additional steps for using a Leap built from source.  For development purposes it is generally advised to use Leap built from source.

#### For building integration tests with Leap built from source

Set an environment variable to tell CDT where to find the Leap build directory:

```sh
export eosio_DIR=/path/to/leap/build/lib/cmake/eosio
export leap_DIR=/path/to/leap/build/lib/cmake/leap
```

### Guided Installation or Building from Scratch
```sh
git clone --recursive https://github.com/AntelopeIO/cdt
cd cdt
mkdir build
cd build
cmake ..
make -j8
```

From here onward you can build your contracts code by simply exporting the `build` directory to your path, so you don't have to install globally (makes things cleaner).
Or you can install globally by running this command:

```sh
sudo make install
```

### Running Tests

#### Unit Tests
```sh
cd build

ctest
```

#### Running Integration Tests (if built)
```sh
cd build/tests/integration

ctest
```

### Uninstall after manual installation

```sh
sudo rm -fr /usr/local/cdt
sudo rm -fr /usr/local/lib/cmake/cdt
sudo rm /usr/local/bin/eosio-*
sudo rm /usr/local/bin/cdt-*
```

## Installed Tools
---
* cdt-cpp
* cdt-cc
* cdt-ld
* cdt-init
* cdt-abidiff
* cdt-wasm2wast
* cdt-wast2wasm
* cdt-ranlib
* cdt-ar
* cdt-objdump
* cdt-readelf

## License

[MIT](./LICENSE)
