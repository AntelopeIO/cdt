#pragma once

#include <eosio/eosio.hpp>

#include <type_traits>
#include <string>
#include <cstddef>
#include <variant>

namespace eosio { namespace testing {
    template <typename Impl>
    struct runner_interface {
        enum class object_type {
            shared_object,
            wasm
        };
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
    
} // testing

namespace rpc { namespace request {
    struct generic {
        uint64_t hash;
    };
    struct divert_account : generic {
        eosio::name name;
    };

    struct return_control_flow : generic {
        eosio::name name;
    };

    struct set_time : generic {
        int64_t time;
    };

    struct get_time : generic {};

    struct call_action : generic {
        eosio::name            receiver;
        eosio::name            code;
        std::vector<std::byte> data;
    };
    struct call_intrinsic : generic {
        int64_t                id;
        std::vector<std::byte> data;
    };

    using message = std::variant<divert_account, return_control_flow, set_time, get_time, call_action, call_intrinsic>;
} //rpc::request

namespace response {
    struct result : request::generic {
        int32_t code;
    };
    struct time_data : request::generic {
        int64_t time;
    };
    struct action_result : request::generic {
        std::vector<std::byte> data;
    };

    using message = std::variant<result, time_data, action_result>;
}} //rpc::response

} //eosio