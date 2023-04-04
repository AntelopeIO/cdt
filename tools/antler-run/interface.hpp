#pragma once

#include <eosio/eosio.hpp>

#include <type_traits>
#include <string>
#include <cstddef>
#include <variant>

namespace eosio { namespace testing {
    enum class object_type {
        shared_object,
        wasm
    };
    template <typename Impl>
    struct runner_interface {
        inline void init() {
            return static_cast<Impl*>(this)->init();
        }
        inline void apply(eosio::name receiver, eosio::name code, eosio::name action) {
            return static_cast<Impl*>(this)->apply(receiver, code, action);
        }
        inline object_type get_type() {
            return static_cast<Impl*>(this)->get_type();
        }
    };
    
}} // eosio::testing