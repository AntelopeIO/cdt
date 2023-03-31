#pragma once

#include <native/eosio/intrinsics.hpp>
#include <native/eosio/crt.hpp>
#include "node-client.hpp"

namespace eosio { namespace native {

    template<eosio::native::intrinsics::intrinsic_name ID, typename Ret, typename TupleArgs, size_t... Is>
    inline auto get_intrinsic_target(std::index_sequence<Is...>) {
        return *eosio::native::intrinsics::get_intrinsic<ID>().template target<Ret(*)(typename std::tuple_element<Is, TupleArgs>::type...)>();
    }

    template<int64_t ID, typename Ret, typename... Args>
    inline auto redirect_to_node(Args... args) {
        return testing::node_client::get().call_intrinsic<ID, Ret>(args...);
    }

    template<int64_t ID, typename Ret, typename TupleArgs, size_t... Is>
    inline auto create_redirect_function(std::index_sequence<Is...>) {
        return std::function<Ret(typename std::tuple_element<Is, TupleArgs>::type ...)>{
            &redirect_to_node<ID, Ret, typename std::tuple_element<Is, TupleArgs>::type ...>
        };
    }

    #define RPC_INTRINSIC(ID) \
    eosio::native::intrinsics::set_intrinsic<eosio::native::intrinsics::ID>( \
        create_redirect_function<eosio::native::intrinsics::ID, \
                                 eosio::native::intrinsics::__ ## ID ## _types::res_t, \
                                 eosio::native::intrinsics::__ ## ID ## _types::deduced_full_ts > \
            (eosio::native::intrinsics::__ ## ID ## _types::is));

    void setup_rpc_intrinsics() {

        INTRINSICS(RPC_INTRINSIC);

        //override print intrinsics to be non-rpc
        eosio::native::set_print_intrinsics();
    }
}} //eosio::testing