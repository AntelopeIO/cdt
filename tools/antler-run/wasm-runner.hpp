#pragma once

#include "utils.hpp"
#include "file-utils.hpp"
#include "interface.hpp"

#include <eosio/vm/backend.hpp>

#include <fstream>
#include <memory>

namespace eosio { namespace wasm {
    struct wasm_runner : testing::runner_interface<wasm_runner> {
        using rhf_t = eosio::vm::registered_host_functions<eosio::vm::standalone_function_t>;
        using backend_t = eosio::vm::backend<rhf_t, eosio::vm::interpreter>;

        wasm_runner(const std::string& path) {
        }

        void apply(eosio::name receiver, eosio::name code, eosio::name action) {
        }
        inline object_type get_type() {
            return testing::runner_interface<wasm_runner>::object_type::wasm;
        }
    private:
        std::unique_ptr<backend_t> backend;

        void load(const std::string& path) {
            auto contract_type = utils::get_file_type(path.c_str());
            if ( contract_type != utils::file_type::wasm ) {
                ANTLER_THROW("not a wasm file: {}", file_type_str(contract_type));
            }

            auto wasm_data = read_wasm(path);
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