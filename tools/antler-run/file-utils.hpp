#pragma once

#include <eosio/vm/constants.hpp>

#include <fstream>
#include <cassert>
#include <vector>
#include <string_view>
#include <cstring>

namespace eosio { namespace utils {

enum class file_type {
    uninitialized,
    non_elf_other,
    elf_object,
    elf_executable,
    elf_shared_object,
    elf_core_dump,
    wasm
};

inline file_type get_file_type(const char* path) {
    std::fstream file;
    file.open(path, std::fstream::in | std::fstream::binary);
    assert(file.is_open());

    std::vector<char> buf(17);
    file.read(buf.data(), buf.size());

    if (buf[0] == 0x7F && 
        buf[1] == 'E' && 
        buf[2] == 'L' && 
        buf[3] == 'F') {
        //ELF binary
        switch (buf[16]) {
            case 1:
            return file_type::elf_object;
            case 2:
            return file_type::elf_executable;
            case 3:
            return file_type::elf_shared_object;
            case 4:
            return file_type::elf_core_dump;
        }
    } else {
        uint32_t wasm_magic;
        memcpy(&wasm_magic, buf.data(), sizeof(wasm_magic));
        if (wasm_magic == eosio::vm::magic)
            return file_type::wasm;
        
        return file_type::non_elf_other;
    }

    return file_type::uninitialized;
}

inline std::string file_type_str(file_type t) {
    switch (t) {
        case file_type::non_elf_other:
        return "non elf";
        case file_type::elf_object:
        return "object";
        case file_type::elf_executable:
        return "executable";
        case file_type::elf_shared_object:
        return "shared object";
        case file_type::elf_core_dump:
        return "core dump";
        case file_type::wasm:
        return "wasm";
        case file_type::uninitialized:
        default:
        return "uninitialized";
    }
}

}} // eosio::utils