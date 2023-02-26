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

struct code_hash {
   uint64_t                    id;
   fc::sha256                  hash;
   uint64_t primary_key()      const { return id; }
};
FC_REFLECT(code_hash, (id)(hash))

BOOST_AUTO_TEST_SUITE(get_code_hash_tests_suite)

BOOST_FIXTURE_TEST_CASE( get_code_hash_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "test"_n, contracts::get_code_hash_write_test_wasm() );
   set_abi( "test"_n,  contracts::get_code_hash_write_test_abi().data() );

   produce_blocks();
   push_action("test"_n, "theaction"_n, "test"_n, mvo());
   code_hash entry;
   get_table_entry(entry, "test"_n, "test"_n, "code.hash"_n, 0);
   wdump((entry.hash));

   set_code( "test"_n, contracts::get_code_hash_read_test_wasm() );
   produce_blocks();

   push_action("test"_n, "theaction"_n, "test"_n, mvo());
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
