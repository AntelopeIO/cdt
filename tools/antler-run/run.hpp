#pragma once

#include "utils.hpp"
#include "options.hpp"
#include "interface.hpp"
#include "node-client.hpp"

#include <filesystem>

namespace eosio { namespace testing {

template <typename Impl>
void run(eosio::testing::runner_interface<Impl>&& runner,
         bool                                     test_only,
         const std::string&                       host = "",
         const std::string&                       port = "",
         eosio::name                              register_account_name = {}) {

    if (test_so_opt) {
        // if we are here, it means contract was loaded succesfully
        const auto& contract_path = contract_path_opt.getValue();
        ANTLER_INFO("{} is valid native contract shared library", std::filesystem::path(contract_path).filename());
        return;
    }

    runner.init();
    // all calls to node_client are synchroneous, if error occurs exception is raised
    // init starts separate connection thread and returns after successful handshake with nodeos
    node_client::get().init(host, port);
    node_client::get().register_account(register_account_name);
    bool running = true;
    while (running) {
        auto response = node_client::get().divert_apply();
        if (response) {
            eosio::name receiver( response->divert_result().receiver() );
            ANTLER_ASSERT( receiver, "receiver name must be set" );
            eosio::name code( response->divert_result().code() );
            ANTLER_ASSERT( code, "code name must be set" );
            eosio::name action( response->divert_result().action() );
            ANTLER_ASSERT( action, "action name must be set" );

            runner.apply(receiver, code, action);
        }
    }
}

}} //eosio::testing