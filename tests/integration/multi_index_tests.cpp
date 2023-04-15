#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include <eosio/testing/tester.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;

#ifdef NON_VALIDATING_TEST
#define TESTER tester
#else
#define TESTER validating_tester
#endif

BOOST_AUTO_TEST_SUITE(multi_index_tests)

// this test is copy from leap test_api_multi_index
BOOST_FIXTURE_TEST_CASE(main_multi_index_tests, TESTER) { try {
   produce_blocks(1);
   create_account( "testapi"_n );
   produce_blocks(1);
   set_code( "testapi"_n, contracts::test_multi_index_wasm() );
   set_abi( "testapi"_n, contracts::test_multi_index_abi().data() );
   produce_blocks(1);

   auto check_failure = [this]( action_name a, const char* expected_error_msg ) {
      BOOST_CHECK_EXCEPTION(  push_action( "testapi"_n, a, "testapi"_n, {} ),
                              eosio_assert_message_exception,
                              eosio_assert_message_is( expected_error_msg )
      );
   };

   push_action( "testapi"_n, "s1g"_n,  "testapi"_n, {} );        // idx64_general
   push_action( "testapi"_n, "s1store"_n,  "testapi"_n, {} );    // idx64_store_only
   push_action( "testapi"_n, "s1check"_n,  "testapi"_n, {} );    // idx64_check_without_storing
   push_action( "testapi"_n, "s2g"_n,  "testapi"_n, {} );        // idx128_general
   push_action( "testapi"_n, "s2store"_n,  "testapi"_n, {} );    // idx128_store_only
   push_action( "testapi"_n, "s2check"_n,  "testapi"_n, {} );    // idx128_check_without_storing
   push_action( "testapi"_n, "s2autoinc"_n,  "testapi"_n, {} );  // idx128_autoincrement_test
   push_action( "testapi"_n, "s2autoinc1"_n,  "testapi"_n, {} ); // idx128_autoincrement_test_part1
   push_action( "testapi"_n, "s2autoinc2"_n,  "testapi"_n, {} ); // idx128_autoincrement_test_part2
   push_action( "testapi"_n, "s3g"_n,  "testapi"_n, {} );        // idx256_general
   push_action( "testapi"_n, "sdg"_n,  "testapi"_n, {} );        // idx_double_general
   push_action( "testapi"_n, "sldg"_n,  "testapi"_n, {} );       // idx_long_double_general

   check_failure( "s1pkend"_n, "cannot increment end iterator" ); // idx64_pk_iterator_exceed_end
   check_failure( "s1skend"_n, "cannot increment end iterator" ); // idx64_sk_iterator_exceed_end
   check_failure( "s1pkbegin"_n, "cannot decrement iterator at beginning of table" ); // idx64_pk_iterator_exceed_begin
   check_failure( "s1skbegin"_n, "cannot decrement iterator at beginning of index" ); // idx64_sk_iterator_exceed_begin
   check_failure( "s1pkref"_n, "object passed to iterator_to is not in multi_index" ); // idx64_pass_pk_ref_to_other_table
   check_failure( "s1skref"_n, "object passed to iterator_to is not in multi_index" ); // idx64_pass_sk_ref_to_other_table
   check_failure( "s1pkitrto"_n, "object passed to iterator_to is not in multi_index" ); // idx64_pass_pk_end_itr_to_iterator_to
   check_failure( "s1pkmodify"_n, "cannot pass end iterator to modify" ); // idx64_pass_pk_end_itr_to_modify
   check_failure( "s1pkerase"_n, "cannot pass end iterator to erase" ); // idx64_pass_pk_end_itr_to_erase
   check_failure( "s1skitrto"_n, "object passed to iterator_to is not in multi_index" ); // idx64_pass_sk_end_itr_to_iterator_to
   check_failure( "s1skmodify"_n, "cannot pass end iterator to modify" ); // idx64_pass_sk_end_itr_to_modify
   check_failure( "s1skerase"_n, "cannot pass end iterator to erase" ); // idx64_pass_sk_end_itr_to_erase
   check_failure( "s1modpk"_n, "updater cannot change primary key when modifying an object" ); // idx64_modify_primary_key
   check_failure( "s1exhaustpk"_n, "next primary key in table is at autoincrement limit" ); // idx64_run_out_of_avl_pk
   check_failure( "s1findfail1"_n, "unable to find key" ); // idx64_require_find_fail
   check_failure( "s1findfail2"_n, "unable to find primary key in require_find" );// idx64_require_find_fail_with_msg
   check_failure( "s1findfail3"_n, "unable to find secondary key" ); // idx64_require_find_sk_fail
   check_failure( "s1findfail4"_n, "unable to find sec key" ); // idx64_require_find_sk_fail_with_msg

   push_action( "testapi"_n, "s1skcache"_n,  "testapi"_n, {} ); // idx64_sk_cache_pk_lookup
   push_action( "testapi"_n, "s1pkcache"_n,  "testapi"_n, {} ); // idx64_pk_cache_sk_lookup

   BOOST_REQUIRE_EQUAL( validate(), true );
} FC_LOG_AND_RETHROW() }

BOOST_AUTO_TEST_SUITE_END()