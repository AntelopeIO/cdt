#include "sync_call_callee.hpp"

#include <eosio/eosio.hpp>
#include <eosio/call.hpp>

class [[eosio::contract]] sync_call_caller : public eosio::contract{
public:
   using contract::contract;

   // Using host function directly
   [[eosio::action]]
   void hstretvaltst() {
      const std::vector<char> data{ eosio::pack("getten"_n.value) };
      auto expected_size = eosio::call("callee"_n.value, 0, data.data(), data.size());
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
      const std::vector<char> data{ eosio::pack(std::make_tuple("getback"_n, 5)) };
      auto expected_size = eosio::call("callee"_n.value, 0, data.data(), data.size());
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
      const std::vector<char> data{ eosio::pack(std::make_tuple("sum"_n, 10, 20, 30)) };
      auto expected_size = eosio::call("callee"_n.value, 0, data.data(), data.size());
      eosio::check(expected_size >= 0, "call did not return a positive value");

      std::vector<char> return_value;
      return_value.resize(expected_size);
      auto actual_size = eosio::get_call_return_value(return_value.data(), return_value.size());
      eosio::check(actual_size == expected_size, "actual_size not equal to expected_size");
      eosio::check(eosio::unpack<uint32_t>(return_value) == 60u, "sum of 10, 20, an 30 not 60");  // sum returns the sum of the 3 arguments
   }

   // Using call_wrapper, testing multiple parameters passing
   [[eosio::action]]
   void wrpmulprmtst() {
      sync_call_callee::sum_func sum{ "callee"_n };
      eosio::check(sum(10, 20, 30) == 60u, "sum of 10, 20, an 30 not 60");
   }

   [[eosio::action]]
   void hstvodfuntst() {
      const std::vector<char> data{ eosio::pack("voidfunc"_n.value) };
      auto expected_size = eosio::call("callee"_n.value, 0, data.data(), data.size());
      eosio::check(expected_size == 0, "call did not return 0"); // void function. return value size should be 0
   }

   [[eosio::action]]
   void wrpvodfuntst() {
      sync_call_callee::voidfunc_func voidfunc{ "callee"_n };
      voidfunc();
   }

   [[eosio::action]]
   void unknwnfuntst() {
      const std::vector<char> data{ eosio::pack("unknwnfunc"_n.value) }; // unknwnfunc is not in "callee"_n contract
      auto expected_size = eosio::call("callee"_n.value, 0, data.data(), data.size());
   }
};
