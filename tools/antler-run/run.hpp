#pragma once

#include "utils.hpp"
#include "options.hpp"
#include "interface.hpp"
#include "node-client.hpp"

#include <filesystem>

namespace eosio { namespace testing {

/// @brief function that perform entire debugging lifecycle
/// @tparam Impl contract execution implementation, currently can be either native or wasm contract runner. 
///              passed as r-value as it is not supposed to live outside run function
/// @param runner contract runner
/// @param host nodeos debug plugin host name
/// @param port nodeos debug plugin port number
/// @param register_account_name account for debugging
template <typename Impl>
void run(runner_interface<Impl>&& runner,
         const std::string&       host = "",
         const std::string&       port = "",
         eosio::name              register_account_name = {}) {

    runner.init();
    // all calls to node_client are synchroneous, if error occurs exception is raised
    // init starts separate connection thread and returns after successful handshake with nodeos
    node_client::get().init(host, port);
    node_client::get().register_account(register_account_name);
    
    // TODO: add signal interruption processing
    //if response is empty that means connection was closed, exiting
    while (auto response = node_client::get().wait_for_divert_flow()) {
        eosio::name receiver( response->divert_flow_result().receiver() );
        ANTLER_ASSERT( receiver, "receiver name must be set" );
        eosio::name code( response->divert_flow_result().code() );
        ANTLER_ASSERT( code, "code name must be set" );
        eosio::name action( response->divert_flow_result().action() );
        ANTLER_ASSERT( action, "action name must be set" );

        runner.apply(receiver, code, action);

        // letting nodeos know that action debugging was done
        node_client::get().return_control_flow(response->req_id());
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