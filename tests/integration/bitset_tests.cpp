#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include <fc/variant_object.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;

using mvo = fc::mutable_variant_object;

BOOST_AUTO_TEST_SUITE(bitset_tests)

BOOST_FIXTURE_TEST_CASE( bitset_test, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();
   set_code( "test"_n, contracts::simple_wasm() );
   set_abi( "test"_n, contracts::simple_abi().data() );
   produce_blocks();

   auto  trx_trace = push_action("test"_n, "testbs"_n, "test"_n, mvo()("b", "0010"));
   auto& act_trace = trx_trace->action_traces[0];
   BOOST_REQUIRE_EQUAL(fc::raw::unpack<fc::bitset>(act_trace.return_value), fc::bitset{"1101"});
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
