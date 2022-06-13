---
content_title: Binary Releases
---

CDT currently supports Linux x86_64 Debian packages.

## Debian Package Install

```sh
wget https://github.com/eoscdt/releases/download/v3.0.0/cdt_3.0.0_amd64.deb
sudo apt install ./cdt_3.0.0_amd64.deb
```

## Debian Package Uninstall

```sh
sudo apt remove cdt
```

# Guided Installation or Building from Scratch

```sh
git clone --recursive https://github.com/eosnetworkfoundation/mandel.cdt
cd eosio.cdt
mkdir build
cd build
cmake ..
make -j8
```

From here onward you can build your contracts code by simply exporting the `build` directory to your path, so you don't have to install globally (makes things cleaner).
Or you can install globally by running this command

```sh
sudo make install
```

## Uninstall after manual installation

```sh
sudo rm -fr /usr/local/cdt
sudo rm -fr /usr/local/lib/cmake/cdt
sudo rm /usr/local/bin/eosio-*
sudo rm /usr/local/bin/cdt-*
```

# Installed Tools

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

License
[MIT](../LICENSE)
