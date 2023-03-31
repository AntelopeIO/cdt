#pragma once

#include "utils.hpp"
#include "file-utils.hpp"
#include "interface.hpp"
#include "intrinsics_setup.hpp"

#include <eosio/action.h>
#include <eosio/chain.h>
#include <eosio/crypto.h>
#include <eosio/db.h>
#include <eosio/permission.h>
#include <eosio/print.h>
#include <eosio/privileged.h>
#include <eosio/system.h>
#include <eosio/transaction.h>

#include <eosio/vm/host_function.hpp>
#include <eosio/vm/backend.hpp>

#include <fstream>
#include <memory>


namespace eosio { namespace wasm {
    struct intrinsics_type_converter : eosio::vm::type_converter<eosio::vm::standalone_function_t> {
    using type_converter::type_converter;
    using type_converter::from_wasm;
    EOS_VM_FROM_WASM(int*, (void* ptr)) { return static_cast<int*>(ptr); }
    EOS_VM_FROM_WASM(long int*, (void* ptr)) { return static_cast<long int*>(ptr); }
    EOS_VM_FROM_WASM(long unsigned int*, (void* ptr)) { return static_cast<long unsigned int*>(ptr); }
    EOS_VM_FROM_WASM(const long unsigned int*, (void* ptr)) { return static_cast<long unsigned int*>(ptr); }
    EOS_VM_FROM_WASM(const __int128 unsigned*, (void* ptr)) { return static_cast<__int128 unsigned*>(ptr); }
    EOS_VM_FROM_WASM(__int128 unsigned*, (void* ptr)) { return static_cast<__int128 unsigned*>(ptr); }
    EOS_VM_FROM_WASM(const __int128*, (void* ptr)) { return static_cast<__int128*>(ptr); }
    EOS_VM_FROM_WASM(__int128*, (void* ptr)) { return static_cast<__int128*>(ptr); }
    EOS_VM_FROM_WASM(const double*, (void* ptr)) { return static_cast<double*>(ptr); }
    EOS_VM_FROM_WASM(double*, (void* ptr)) { return static_cast<double*>(ptr); }
    EOS_VM_FROM_WASM(const long double*, (void* ptr)) { return static_cast<long double*>(ptr); }
    EOS_VM_FROM_WASM(long double*, (void* ptr)) { return static_cast<long double*>(ptr); }
    EOS_VM_FROM_WASM(const capi_checksum512*, (void* ptr)) { return static_cast<capi_checksum512*>(ptr); }
    EOS_VM_FROM_WASM(capi_checksum512*, (void* ptr)) { return static_cast<capi_checksum512*>(ptr); }
    EOS_VM_FROM_WASM(const capi_checksum256*, (void* ptr)) { return static_cast<capi_checksum256*>(ptr); }
    EOS_VM_FROM_WASM(capi_checksum256*, (void* ptr)) { return static_cast<capi_checksum256*>(ptr); }
    EOS_VM_FROM_WASM(const capi_checksum160*, (void* ptr)) { return static_cast<capi_checksum160*>(ptr); }
    EOS_VM_FROM_WASM(capi_checksum160*, (void* ptr)) { return static_cast<capi_checksum160*>(ptr); }
    EOS_VM_FROM_WASM(bool, (uint32_t value)) { return value ? 1 : 0; }
    EOS_VM_FROM_WASM(char*, (void* ptr)) { return static_cast<char*>(ptr); }
    EOS_VM_FROM_WASM(const char*, (void* ptr)) { return static_cast<char*>(ptr); }
    };

    struct runner : testing::runner_interface<runner> {
        using rhf_t = eosio::vm::registered_host_functions<eosio::vm::standalone_function_t, eosio::vm::execution_interface, intrinsics_type_converter>;
        using backend_t = eosio::vm::backend<rhf_t, eosio::vm::interpreter>;

        #define REGISTER_WASM_INTRINSIC(ID) \
            rhf_t::add<&::ID>("host", #ID);

        runner(const std::string& path) {
            load(path);
        }

        void apply(eosio::name receiver, eosio::name code, eosio::name action) {
            backend->call("env", "apply", receiver.value, code.value, action.value);
        }
        inline object_type get_type() {
            return testing::runner_interface<runner>::object_type::wasm;
        }
        void init() {
            eosio::native::setup_rpc_intrinsics();
            
            INTRINSICS(REGISTER_WASM_INTRINSIC);
        }
    private:
        eosio::vm::wasm_allocator wasm_allocator;
        std::unique_ptr<backend_t> backend;

        void load(const std::string& path) {
            auto contract_type = utils::get_file_type(path.c_str());
            if ( contract_type != utils::file_type::wasm ) {
                ANTLER_THROW("not a wasm file: {}", file_type_str(contract_type));
            }

            auto wasm_data = read_wasm(path);
            backend.reset( new backend_t(wasm_data, &wasm_allocator) );
        }

        std::vector<uint8_t> read_wasm( const std::string& fname ) {
            std::ifstream wasm_file(fname, std::ios::binary);
            ANTLER_ASSERT( wasm_file.is_open(), "error opening file: {}", fname );
            
            wasm_file.seekg(0, std::ios::end);
            int len = wasm_file.tellg();
            ANTLER_ASSERT( len, "wasm file is empty", fname );
            ANTLER_ASSERT( len != -1, "file error: {}", wasm_file.rdstate() );

            std::vector<uint8_t> wasm;
            wasm.resize(len);

            wasm_file.seekg(0, std::ios::beg);
            wasm_file.read((char*)wasm.data(), wasm.size());
            wasm_file.close();

            return std::move(wasm);
        }
    };
}} // eosio::testing