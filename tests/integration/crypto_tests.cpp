#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;

using mvo = fc::mutable_variant_object;

BOOST_AUTO_TEST_SUITE(crypto_primitives_tests)

BOOST_FIXTURE_TEST_CASE( sha3_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "sha3test"_n, "test"_n, mvo()
      ("val", "hello")
      ("sha3_dg", "3338be694f50c5f338814986cdf0686453a888b84f424d792af4b9202398f392"));

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( simple_eosio_tests, tester ) try {
   set_code( "eosio"_n, contracts::simple_wasm() );
   set_abi( "eosio"_n,  contracts::simple_wrong_abi().data() );
   produce_blocks();
   push_action("eosio"_n, "test1"_n, "eosio"_n,
         mvo()
         ("nm", "bucky"));

   BOOST_CHECK_THROW(push_action("eosio"_n, "test1"_n, "eosio"_n, mvo()("nm", "notbucky")),
         fc::exception);

   push_action("eosio"_n, "test2"_n, "eosio"_n,
         mvo()
         ("arg0", 33)
         ("arg1", "some string"));
   BOOST_CHECK_THROW(push_action("eosio"_n, "test2"_n, "eosio"_n, mvo() ("arg0", 30)("arg1", "some string")), fc::exception);
   BOOST_CHECK_THROW(push_action("eosio"_n, "test2"_n, "eosio"_n, mvo() ("arg0", 33)("arg1", "not some string")), fc::exception);

   push_action("eosio"_n, "test3"_n, "eosio"_n,
         mvo()
         ("arg0", 33)
         ("arg1", "some string"));

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
