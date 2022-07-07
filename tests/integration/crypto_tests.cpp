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

   push_action("eosio"_n, "sha3test"_n, "test"_n, mvo()
      ("val", "")
      ("sha3_dg", "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a"));

   push_action("eosio"_n, "sha3test"_n, "test"_n, mvo()
      ("val", "mandelmandelmandel")
      ("sha3_dg", "0397431c4c41f7b349e053bda6822521a32dde36e2f7522c0cd6aab1a97ed0dc"));

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( keccak_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "keccaktest"_n, "test"_n, mvo()
      ("val", "hello")
      ("sha3_dg", "1c8aff950685c2ed4bc3174f3472287b56d9517b9c948127319a09a7a36deac8"));

   push_action("eosio"_n, "keccaktest"_n, "test"_n, mvo()
      ("val", "")
      ("sha3_dg", "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470"));

   push_action("eosio"_n, "keccaktest"_n, "test"_n, mvo()
      ("val", "mandelmandelmandel")
      ("sha3_dg", "75d45d9bdb758f80743e0e6d90b3fcd786bbab5089af37797e18223cf5f6eb0c"));

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()