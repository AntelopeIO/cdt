# CDT (Contract Development Toolkit)
## Version : 3.0.0

CDT is a toolchain for WebAssembly (WASM) and a set of tools to facilitate smart contract development for the Mandel platform. In addition to being a general purpose WebAssembly toolchain, [Mandel](https://github.com/eosnetworkfoundation/mandel) specific optimizations are available to support building smart contracts.  This new toolchain is built around [Clang 9](https://github.com/eosnetworkfoundation/cdt-llvm), which means that CDT has the most currently available optimizations and analyses from LLVM, but as the WASM target is still considered experimental, some optimizations are incomplete or not available.

## Binary Releases
CDT currently supports Linux x86_64 Debian packages.
### Debian Package Install
```sh
wget https://github.com/eosnetworkfoundation/mandel.cdt/releases/download/v3.0.0/cdt_3.0.0_amd64.deb
sudo apt install ./cdt_3.0.0_amd64.deb
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

#### For building integration tests with mandel built from source (not installed)

Need to provide some environment variables to find necessary test dependencies
Currently need both while mandel repo is undergoing evolution in naming.

```sh
export PATH=${mandel_root}/build/bin
```

### Guided Installation or Building from Scratch
```sh
git clone --recursive https://github.com/eosnetworkfoundation/mandel.cdt
cd mandel.cdt
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