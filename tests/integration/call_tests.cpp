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

   // Using host function directly
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "hstretvaltst"_n, "caller"_n, {}));

   // Using call_wrapper
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "wrpretvaltst"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify one parameter passing works correctly
BOOST_AUTO_TEST_CASE(param_basic_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   // Using host function directly
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "hstoneprmtst"_n, "caller"_n, {}));

   // Using call_wrapper
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "wrponeprmtst"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify multiple parameters passing works correctly
BOOST_AUTO_TEST_CASE(multiple_params_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   // Using host function directly
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "hstmulprmtst"_n, "caller"_n, {}));

   // Using call_wrapper
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "wrpmulprmtst"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify passing a struct parameter works correctly
BOOST_AUTO_TEST_CASE(struct_param_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "structtest"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify passing a mix of structs and integer works correctly
BOOST_AUTO_TEST_CASE(mix_struct_int_params_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "structinttst"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify a sync call to a void function works properly.
BOOST_AUTO_TEST_CASE(void_func_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   auto check = [] (const transaction_trace_ptr& trx_trace) {
      auto& atrace    = trx_trace->action_traces;

      auto& call_traces  = atrace[0].call_traces;
      BOOST_REQUIRE_EQUAL(call_traces.size(), 1u);

      // Verify the print from the void function is correct.
      // The test contract checks the return value size is 0.
      auto& call_trace = call_traces[0];
      BOOST_REQUIRE_EQUAL(call_trace.call_ordinal, 1u);
      BOOST_REQUIRE_EQUAL(call_trace.sender_ordinal, 0u);
      BOOST_REQUIRE_EQUAL(call_trace.console, "I am a void function");
   };

   // Using host function directly
   auto trx_trace = t.push_action("caller"_n, "hstvodfuntst"_n, "caller"_n, {});
   check(trx_trace);

   // Using call_wrapper
   trx_trace = t.push_action("caller"_n, "wrpvodfuntst"_n, "caller"_n, {});
   check(trx_trace);
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
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "hstmulprmtst"_n, "caller"_n, {}));

   // Make sure we can push an action using `sum`.
   //BOOST_REQUIRE_NO_THROW(t.push_action("callee"_n, "sum"_n, "callee"_n,
   t.push_action("callee"_n, "sum"_n, "callee"_n,
                                        mvo()
                                             ("a", 1)
                                             ("b", 2)
                                             ("c", 3)); //);
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
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "hstretvaltst"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify support_mode for void and non-void sync calls if calls are a failure
BOOST_AUTO_TEST_CASE(sync_call_support_mode_failure_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::not_supported_wasm(), contracts::not_supported_abi().data()}
   });

   // voidfncnoop uses support_mode::no_op
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "voidfncnoop"_n, "caller"_n, {}));

   // voidfncabort uses default support_mode::abort
   BOOST_CHECK_EXCEPTION(t.push_action("caller"_n, "voidfncabort"_n, "caller"_n, {}),
                         eosio_assert_message_exception,
                         fc_exception_message_contains("receiver does not support sync call but support_mode is set to abort"));

   // intfuncnoop uses support_mode::no_op
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "intfuncnoop"_n, "caller"_n, {}));

   // intfuncabort uses default support_mode::abort
   BOOST_CHECK_EXCEPTION(t.push_action("caller"_n, "intfuncabort"_n, "caller"_n, {}),
                         eosio_assert_message_exception,
                         fc_exception_message_contains("receiver does not support sync call but support_mode is set to abort"));
} FC_LOG_AND_RETHROW() }

// Verify support_mode for void and non-void sync calls if call is successful
BOOST_AUTO_TEST_CASE(sync_call_support_mode_success_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   // voidnoopsucc uses support_mode::no_op
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "voidnoopsucc"_n, "caller"_n, {}));

   // sumnoopsucc uses support_mode::no_op
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "sumnoopsucc"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify header validation
BOOST_AUTO_TEST_CASE(unknown_function_test) { try {
   call_tester t({
      {"caller"_n, contracts::caller_wasm(), contracts::caller_abi().data()},
      {"callee"_n, contracts::callee_wasm(), contracts::callee_abi().data()}
   });

   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "hdrvaltest"_n, "caller"_n, {}));
} FC_LOG_AND_RETHROW() }

// Verify adding/reading entries to/from a table, and read-only enforcement work
BOOST_AUTO_TEST_CASE(addr_book_tests) { try {
   call_tester t({
      {"caller"_n, contracts::addr_book_caller_wasm(), contracts::addr_book_caller_abi().data()},
      {"callee"_n, contracts::addr_book_callee_wasm(), contracts::addr_book_callee_abi().data()}
   });

   // Try to add an entry using a read-only sync call
   BOOST_CHECK_EXCEPTION(t.push_action("caller"_n, "upsertrdonly"_n, "caller"_n, mvo()
                            ("user", "alice")
                            ("first_name", "alice")
                            ("street", "123 Main St.")),
                         unaccessible_api,
                         fc_exception_message_contains("this API is not allowed in read only action/call"));

   // Add an entry using a read-write sync call
   t.push_action("caller"_n, "upsert"_n, "caller"_n, mvo()
      ("user", "alice")
      ("first_name", "alice")
      ("street", "123 Main St."));

   // Read the inserted entry. "get"_n action will check the return value from the sync call
   BOOST_REQUIRE_NO_THROW(t.push_action("caller"_n, "get"_n, "caller"_n, mvo() ("user", "alice")));
} FC_LOG_AND_RETHROW() }

BOOST_AUTO_TEST_SUITE_END()
