#pragma once

#include "utils.hpp"
#include "options.hpp"
#include "interface.hpp"
#include "node-client.hpp"

#include <filesystem>

namespace eosio { namespace testing {

template <typename Impl>
void run(const eosio::testing::runner_interface<Impl>& runner) {

    if (test_so_opt) {
        // if we are here, it means contract was loaded succesfully
        const auto& contract_path = contract_path_opt.getValue();
        ANTLER_INFO("{} is valid native contract shared library", std::filesystem::path(contract_path).filename());
        return;
    }
}

}} //eosio::testing