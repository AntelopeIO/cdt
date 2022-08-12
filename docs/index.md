# EOSIO.CDT (Contract Development Toolkit)

EOSIO.CDT is a toolchain for WebAssembly (WASM) and set of tools to facilitate smart contract development for the EOSIO platform. In addition to being a general purpose WebAssembly toolchain, [Leap](https://github.com/AntelopeIO/leap) specific optimizations are available to support building EOSIO smart contracts.  This new toolchain is built around [Clang 9](https://github.com/AntelopeIO/cdt-llvm), which means that EOSIO.CDT has the most currently available optimizations and analyses from LLVM, but as the WASM target is still considered experimental, some optimizations are incomplete or not available.

## Upgrading

There's been a round of breaking changes, if you are upgrading from an older version please read [Upgrading CDT to Leap](./04_upgrading/eosio.cdt-to-leap.cdt.md).

## Contributing

See the [Contributing Guide](https://github.com/eosnetworkAntelopeIO/leap/blob/main/CONTRIBUTING.md)

## License

[MIT](./../LICENSE)

## Important

See LICENSE for copyright and license terms.  Block.one makes its contribution on a voluntary basis as a member of the EOSIO community and is not responsible for ensuring the overall performance of the software or any related applications.  We make no representation, warranty, guarantee or undertaking in respect of the software or any related documentation, whether expressed or implied, including but not limited to the warranties or merchantability, fitness for a particular purpose and noninfringement. In no event shall we be liable for any claim, damages or other liability, whether in an action of contract, tort or otherwise, arising from, out of or in connection with the software or documentation or the use or other dealings in the software or documentation.  Any test results or performance figures are indicative and will not reflect performance under all conditions.  Any reference to any third party or third-party product, service or other resource is not an endorsement or recommendation by Block.one.  We are not responsible, and disclaim any and all responsibility and liability, for your use of or reliance on any of these resources. Third-party resources may be updated, changed or terminated at any time, so the information here may be out of date or inaccurate.

