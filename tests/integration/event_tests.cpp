#include <boost/test/unit_test.hpp>

#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <fc/variant_object.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;
using namespace fc;

using mvo = fc::mutable_variant_object;

struct acct_and_code {
   account_name          acct;
   std::vector<uint8_t>  wasm;
   char*                 abi = nullptr;
};

BOOST_AUTO_TEST_SUITE(event_tests)

// Verify a sync call returns value correctly
BOOST_AUTO_TEST_CASE(emit_event) {
   try {
      tester t;
      auto alice = "alice"_n;
      create_account(alice);
      set_code(alice, contracts::event_wasm());
      set_abi(alice, contracts::event__abi().data());
      BOOST_REQUIRE_NO_THROW(t.push_action(alice, "bob"_n, alice, {}));
   }
   FC_LOG_AND_RETHROW()
}
