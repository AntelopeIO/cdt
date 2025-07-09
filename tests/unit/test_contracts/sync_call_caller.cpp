#include "sync_call_callee.hpp"
#include "sync_call_not_supported.hpp"

#include <eosio/eosio.hpp>
#include <eosio/call.hpp>

using namespace eosio;

class [[eosio::contract]] sync_call_caller : public eosio::contract{
public:
   using contract::contract;

   // Using host function directly
   [[eosio::action]]
   void hstretvaltst() {
      call_data_header header{ .version = 0, .func_name = "getten"_n.value };
      const std::vector<char> data{ eosio::pack(header) };
      auto expected_size = eosio::call("callee"_n, 0, data.data(), data.size());
      eosio::check(expected_size >= 0, "call did not return a positive value");

      std::vector<char> return_value;
      return_value.resize(expected_size);
      auto actual_size = eosio::get_call_return_value(return_value.data(), return_value.size());
      eosio::check(actual_size == expected_size, "actual_size not equal to expected_size");
      eosio::check(eosio::unpack<uint32_t>(return_value) == 10u, "return value not 10");  // getten always returns 10
   }

   // Using call_wrapper
   [[eosio::action]]
   void wrpretvaltst() {
      sync_call_callee::getten_func getten{ "callee"_n };
      eosio::check(getten() == 10u, "return value not 10");
   }

   // Using host function directly, testing one parameter passing
   [[eosio::action]]
   void hstoneprmtst() {
      // `getback(uint32_t p)` returns p
      call_data_header header{ .version = 0, .func_name = "getback"_n.value };
      const std::vector<char> data{ eosio::pack(std::make_tuple(header, 5)) };
      auto expected_size = eosio::call("callee"_n, 0, data.data(), data.size());
      eosio::check(expected_size >= 0, "call did not return a positive value");

      std::vector<char> return_value;
      return_value.resize(expected_size);
      auto actual_size = eosio::get_call_return_value(return_value.data(), return_value.size());
      eosio::check(actual_size == expected_size, "actual_size not equal to expected_size");
      eosio::check(eosio::unpack<uint32_t>(return_value) == 5u, "return value not 5");  // getback returns back the same value of parameter
   }

   // Using call_wrapper, testing one parameter passing
   [[eosio::action]]
   void wrponeprmtst() {
      sync_call_callee::getback_func getback{ "callee"_n };
      eosio::check(getback(5) == 5u, "return value not 5");
   }

   // Using host function directly, testing multiple parameters passing
   [[eosio::action]]
   void hstmulprmtst() {
      call_data_header header{ .version = 0, .func_name = "sum"_n.value };
      const std::vector<char> data{ eosio::pack(std::make_tuple(header, 10, 20, 30)) };
      auto expected_size = eosio::call("callee"_n, 0, data.data(), data.size());
      eosio::check(expected_size >= 0, "call did not return a positive value");

      std::vector<char> return_value;
      return_value.resize(expected_size);
      auto actual_size = eosio::get_call_return_value(return_value.data(), return_value.size());
      eosio::check(actual_size == expected_size, "actual_size not equal to expected_size");
      eosio::check(eosio::unpack<uint32_t>(return_value) == 60u, "sum of 10, 20, and 30 not 60");  // sum returns the sum of the 3 arguments
   }

   // Using call_wrapper, testing multiple parameters passing
   [[eosio::action]]
   void wrpmulprmtst() {
      sync_call_callee::sum_func sum{ "callee"_n };
      eosio::check(sum(10, 20, 30) == 60u, "sum of 10, 20, and 30 not 60");
   }

   // Verify single struct parameter passing
   [[eosio::action]]
   void structtest() {
      sync_call_callee::structonly_func func{ "callee"_n };
      struct1_t input = { 10, 20 };
      auto output = func(input); // structonly_func returns the input as is
      eosio::check(output.a == input.a, "field a in output is not equal to a in input");
      eosio::check(output.b == input.b, "field b in output is not equal to b in input");
   }

   // Verify mix of struct and integer parameters passing
   [[eosio::action]]
   void structinttst() {
      sync_call_callee::structmix_func func{ "callee"_n };
      struct1_t input1 = { 10, 20 };
      struct2_t input2 = { 'a', true, 50, 100 };
      int32_t m = 2;

      // structmix_func multiply each field of input1 by m,
      // add last two fields of input2, and return a struct1_t
      auto output = func(input1, m, input2);
      eosio::check(output.a == m * input1.a + input2.c, "field a of output is not correct");
      eosio::check(output.b == m * input1.b + input2.d, "field b of output is not correct");
   }

   [[eosio::action]]
   void hstvodfuntst() {
      call_data_header header{ .version = 0, .func_name = "voidfunc"_n.value };
      const std::vector<char> data{ eosio::pack(header) };
      auto expected_size = eosio::call("callee"_n, 0, data.data(), data.size());
      eosio::check(expected_size == 0, "call did not return 0"); // void function. return value size should be 0
   }

   [[eosio::action]]
   void wrpvodfuntst() {
      sync_call_callee::voidfunc_func voidfunc{ "callee"_n };
      voidfunc();
   }

   // Verify void call. void_func uses default support_mode::abort
   [[eosio::action]]
   void voidfncabort() {
      sync_call_not_supported::void_func void_func_abort{ "callee"_n };
      void_func_abort();  // Will throw. Tester will verify that.
   }

   // void_func uses support_mode::no_op
   [[eosio::action]]
   void voidfncnoop() {
      sync_call_not_supported::void_no_op_func void_func_no_op{ "callee"_n };
      check(void_func_no_op() == std::nullopt, "void_func_no_op did not return std::nullopt");
   }

   // verify non-void call. int_func uses default support_mode::abort
   [[eosio::action]]
   void intfuncabort() {
      sync_call_not_supported::int_func int_func_abort{ "callee"_n };
      int_func_abort(); // Will throw. Tester will verify that.
   }

   // int_func uses support_mode::no_opabort
   [[eosio::action]]
   void intfuncnoop() {
      sync_call_not_supported::int_no_op_func int_func_no_op{ "callee"_n };
      check(int_func_no_op() == std::nullopt, "void_func_no_op did not return std::nullopt");
   }

   // void_no_op_success_func uses support_mode::no_op
   [[eosio::action]]
   void voidnoopsucc() {
      sync_call_callee::void_no_op_success_func f{ "callee"_n };
      check(f().has_value(), "void_no_op_success_func did not return a value");
   }

   // void_no_op_success_func uses support_mode::no_op
   [[eosio::action]]
   void sumnoopsucc() {
      sync_call_callee::sum_no_op_success_func f{ "callee"_n };
      check(*f(7, 8, 9) == 24, "sum_no_op_success_func did not return a value");
   }

   [[eosio::action]]
   void hdrvaltest() {
      // Verify function name validation works
      call_data_header unkwn_func_header{ .version = 0, .func_name = "unknwnfunc"_n.value };
      const std::vector<char> unkwn_func_data{ eosio::pack(unkwn_func_header) }; // unknwnfunc is not in "callee"_n contract
      auto status = eosio::call("callee"_n, 0, unkwn_func_data.data(), unkwn_func_data.size());
      eosio::check(status == -10001, "call did not return -10001 for unknown function");

      // Verify version validation works
      call_data_header bad_version_header{ .version = 1, .func_name = "sum"_n.value };  // version 1 is not supported
      const std::vector<char> bad_version_data{ eosio::pack(bad_version_header) };
      status = eosio::call("callee"_n, 0, bad_version_data.data(), bad_version_data.size());
      eosio::check(status == -10000, "call did not return -10000 for invalid version");
   }
};
