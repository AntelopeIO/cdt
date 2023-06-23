#include <boost/test/unit_test.hpp>
#include "../../tools/antler-run/wasm-runner.hpp"
#include "contracts.hpp"

BOOST_AUTO_TEST_SUITE(antler_run_wasm_test)

BOOST_AUTO_TEST_CASE(apply) {

    try {
        // this is gonna fail because of pointers type mismatch in memcpy
        // to fix this you need to change intrinsics class as currently it can't deal with overrides
        // one of ideas to do this is to put everything in interface namespace to avoid interference with global overrides
        eosio::testing::wasm::runner runner(eosio::testing::contracts::action_results_test_path());
    } catch (eosio::vm::exception& ex) {
        BOOST_TEST_MESSAGE(ex.what());
        BOOST_TEST_MESSAGE(ex.detail());
        throw;
    }

}

BOOST_AUTO_TEST_SUITE_END()