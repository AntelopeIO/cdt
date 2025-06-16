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

BOOST_AUTO_TEST_SUITE(array_tests)

BOOST_FIXTURE_TEST_CASE( std_array_param, tester ) try {
   /* -----------  testpa action tests --------------------------------------------------
   [[eosio::action]]
   void testpa(std::array<int,4> input){
      std::array<int,4> arr = input;
      for(int i = 0; i < 4; ++i){
         eosio::cout << arr[i] << " ";
      }
      eosio::cout << "\n";
   }
   -------------------------------------------------------------------------------------- */
   create_accounts( { "test"_n } );
   produce_block();
   set_code( "test"_n, contracts::array_tests_wasm() );
   set_abi( "test"_n, contracts::array_tests_abi().data() );
   produce_blocks();

   auto  trace    = push_action("test"_n, "testpa"_n, "test"_n, mvo()("input", {1,2,3,4}));
   auto& con      = trace->action_traces[0].console;
   BOOST_REQUIRE_EQUAL(con, std::string("1 2 3 4 \n"));
   produce_block();

   // size should be correct
   // ----------------------
   BOOST_CHECK_EXCEPTION( push_action("test"_n, "testpa"_n, "test"_n, mvo()("input", {1,2,3})),
                          pack_exception,
                          fc_exception_message_starts_with("Incorrect number of values provided (4) for fixed-size (3) array type"));

   produce_block();
} FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE( std_array_return_value, tester ) try {
   /* -----------  testre action tests --------------------------------------------------
   [[eosio::action]]
   std::array<int,4> testre(std::array<int,4> input){
      std::array<int,4> arr = input;
      for(auto & v : arr) v += 1;
      return arr;
   }
   -------------------------------------------------------------------------------------- */
   create_accounts( { "test"_n } );
   produce_block();
   set_code( "test"_n, contracts::array_tests_wasm() );
   set_abi( "test"_n, contracts::array_tests_abi().data() );
   produce_blocks();

   auto  trace    = push_action("test"_n, "testre"_n, "test"_n, mvo()("input", {1, 2, 3, 4}));
   auto& rv       = trace->action_traces[0].return_value;
   auto  actual   = fc::raw::unpack<std::array<int, 4>>(rv);
   auto  expected = std::array<int, 4>{2, 3, 4, 5};
   BOOST_REQUIRE(actual == expected);
} FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE( std_vector_return_value, tester ) try {
   /* -----------  testrev action tests --------------------------------------------------
   [[eosio::action]]
   std::vector<int> testrev(std::vector<int> input){
      std::vector<int> vec = input;
      for(auto & v : vec) v += 1;
      return vec;
   }
   -------------------------------------------------------------------------------------- */
   create_accounts( { "test"_n } );
   produce_block();
   set_code( "test"_n, contracts::array_tests_wasm() );
   set_abi( "test"_n, contracts::array_tests_abi().data() );
   produce_blocks();

   auto  trace    = push_action("test"_n, "testrev"_n, "test"_n, mvo()("input", {1, 2, 3, 4}));
   auto& rv       = trace->action_traces[0].return_value;
   auto  actual   = fc::raw::unpack<std::vector<int>>(rv);
   auto  expected = std::vector<int>{2, 3, 4, 5};
   BOOST_REQUIRE(actual == expected);
} FC_LOG_AND_RETHROW()



BOOST_AUTO_TEST_SUITE_END()
