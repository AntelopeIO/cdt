#include <eosio/eosio.hpp>
#include <eosio/call.hpp>

class [[eosio::contract]] sync_call_caller : public eosio::contract{
public:
   using contract::contract;

   [[eosio::action]]
   void retvaltest() {
      eosio::name func_name = "getten"_n;
      auto expected_size = eosio::call("callee"_n, "getten"_n)();
      eosio::check(expected_size >= 0, "call did not return a positive value");

      std::vector<char> return_value;
      return_value.resize(expected_size);
      auto actual_size = eosio::get_call_return_value(return_value.data(), return_value.size());
      eosio::check(actual_size == expected_size, "actual_size not equal to expected_size");
      eosio::check(eosio::unpack<uint32_t>(return_value), 10u);  // getten always returns 10
   }

   [[eosio::action]]
   void paramtest() {
      eosio::name func_name = "getten"_n;
      // `getback(uint32_t p)` returns p
      auto expected_size = eosio::call("callee"_n, std::make_tuple("getback"_n, 5))();
      eosio::check(expected_size >= 0, "call did not return a positive value");

      std::vector<char> return_value;
      return_value.resize(expected_size);
      auto actual_size = eosio::get_call_return_value(return_value.data(), return_value.size());
      eosio::check(actual_size == expected_size, "actual_size not equal to expected_size");
      eosio::check(eosio::unpack<uint32_t>(return_value), 5u);  // getback returns back the same value of parameter
   }
};
