#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <eosio/chain/abi_serializer.hpp>
#include <eosio/testing/tester.hpp>

//#include <fc/variant_object.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;

struct kv_tester {
   kv_tester(std::vector<uint8_t> wasm, std::vector<char> abi) {

      chain.create_accounts({"kvtest"_n});
      chain.produce_block();
      chain.set_code("kvtest"_n, wasm);
      chain.set_abi("kvtest"_n, abi.data());
      chain.produce_blocks();
   }

   void push_action(name act, std::string exception_msg="") {
      if (exception_msg.empty()) {
         chain.push_action("kvtest"_n, act, "kvtest"_n, {});
      } else {
         BOOST_CHECK_EXCEPTION(chain.push_action("kvtest"_n, act, "kvtest"_n, {}),
                               eosio_assert_message_exception,
                               eosio_assert_message_is(exception_msg));
      }
   }

   tester chain;
};

BOOST_AUTO_TEST_SUITE(key_value_tests)

BOOST_AUTO_TEST_CASE(map_tests) try {
   kv_tester t = {contracts::kv_map_tests_wasm(), contracts::kv_map_tests_abi()};
   t.push_action("test"_n);
   t.push_action("iter"_n);
   t.push_action("erase"_n);
   t.push_action("eraseexcp"_n, "key not found");
   t.push_action("bounds"_n);
   t.push_action("ranges"_n);
   t.push_action("empty"_n);
   t.push_action("gettmpbuf"_n);
   t.push_action("constrct"_n);
   t.push_action("keys"_n);
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()