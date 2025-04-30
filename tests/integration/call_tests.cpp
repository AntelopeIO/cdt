#include <boost/test/unit_test.hpp>

#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <fc/variant_object.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;
using namespace fc;

using mvo = fc::mutable_variant_object;

BOOST_AUTO_TEST_SUITE(call_tests)

BOOST_FIXTURE_TEST_CASE(basic_test, tester) try {
    create_accounts({ "calltests"_n });
    produce_block();

    set_code("calltests"_n, contracts::call_test_wasm());
    set_abi("calltests"_n, contracts::call_test_abi().data());

    produce_block();

    BOOST_REQUIRE_NO_THROW(push_action("calltests"_n, "basictest"_n, "calltests"_n, {}));
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
