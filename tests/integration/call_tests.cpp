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

// The first account in the accounts vector is the action initiating the
// first sync call
struct call_tester: tester {
   call_tester(const std::vector<acct_and_code>& accounts) {
      for (auto i = 0u; i < accounts.size(); ++i) {
         create_account(accounts[i].acct);
         set_code(accounts[i].acct, accounts[i].wasm);
         set_abi(accounts[i].acct, accounts[i].abi);
      }

      produce_block();
   }
};

BOOST_AUTO_TEST_SUITE(sync_call_tests)

// Verify a sync call returns value correctly
BOOST_AUTO_TEST_CASE(return_value_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "retvaltest"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify one parameter passing works correctly
BOOST_AUTO_TEST_CASE(param_basic_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "paramtest"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify multiple parameters passing works correctly
BOOST_AUTO_TEST_CASE(multiple_params_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "mulparamtest"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify a sync call to a void function works properly.
BOOST_AUTO_TEST_CASE(void_func_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   auto  trx_trace = t.push_action("caller"_n, "voidfunctest"_n, "caller"_n, {});
   auto& atrace    = trx_trace->action_traces;

   auto& call_traces  = atrace[0].call_traces;
   BOOST_REQUIRE_EQUAL(call_traces.size(), 1u);

   // Verify the print from the void function is correct.
   // The test contract checks the return value size is 0.
   auto& call_trace = call_traces[0];
   BOOST_REQUIRE_EQUAL(call_trace.call_ordinal, 1u);
   BOOST_REQUIRE_EQUAL(call_trace.sender_ordinal, 0u);
   BOOST_REQUIRE_EQUAL(call_trace.console, "I am a void function");
} FC_LOG_AND_RETHROW() }

// Verify a function tagged as both `action` and `call` works
BOOST_AUTO_TEST_CASE(mixed_action_call_tags_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   // `sum` in `callee` contract is tagged as `action` and `call`

   // Make sure we can make a sync call to `sum` (`mulparamtest` in `caller` does
   // a sync call to `sum`)
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "mulparamtest"_n, "caller"_n, {}));

   // Make sure we can push an action using `sum`.
   BOOST_REQUIRE_NO_THROW(t.push_action("callee"_n, "sum"_n, "callee"_n,
                                        mvo()
                                             ("in1", 1)
                                             ("in2", 2)
                                             ("in3", 3)));
} FC_LOG_AND_RETHROW() }

// Verify the receiver contract with only one sync call function works
// (for testing the sync_call entry point dispatcher)
BOOST_AUTO_TEST_CASE(single_function_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::single_func_wasm(), contracts::single_func_abi().data()}
   });

   // The single_func_wasm contains only one function and the caller contract
   // hooks up with it
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "retvaltest"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify no_op_if_receiver_not_support_sync_call flag works
BOOST_AUTO_TEST_CASE(sync_call_not_supported_test) { try {
   call_tester t({
      {"caller"_n, contracts::not_supported_wasm(), contracts::not_supported_abi().data()}
   });

   // * sync_call_not_supported contract only has actions
   // * no_op_if_receiver_not_support_sync_call is set
   // so the call is just a no-op
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "noopset"_n, "caller"_n, {}));

   // * sync_call_not_supported contract only has actions
   // * no_op_if_receiver_not_support_sync_call is NOT set
   // so the call aborts
   BOOST_CHECK_EXCEPTION(t.push_action("caller"_n, "noopnotset"_n, "caller"_n, {}),
                         eosio_assert_message_exception,
                         fc_exception_message_contains("receiver does not support sync call but no_op_if_receiver_not_support_sync_call flag is not set"));
} FC_LOG_AND_RETHROW() }

// Verify calling an unknown function will result in an eosio_assert
BOOST_AUTO_TEST_CASE(unknown_function_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   BOOST_CHECK_EXCEPTION(t.push_action("caller"_n, "unknwnfuntst"_n, "caller"_n, {}),
                         eosio_assert_code_exception,
                         eosio_assert_code_is(8000000000000000003));
} FC_LOG_AND_RETHROW() }

BOOST_AUTO_TEST_SUITE_END()
