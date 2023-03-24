#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;
using namespace eosio::chain;
using namespace fc;

using mvo = fc::mutable_variant_object;

BOOST_AUTO_TEST_SUITE(name_pk_tests_suite)

BOOST_FIXTURE_TEST_CASE( name_pk_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "test"_n, contracts::name_pk_tests_wasm() );
   set_abi( "test"_n,  contracts::name_pk_tests_abi().data() );

   produce_blocks();
   push_action("test"_n, "write"_n, "test"_n, mvo());
   push_action("test"_n, "read"_n, "test"_n, mvo());
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
