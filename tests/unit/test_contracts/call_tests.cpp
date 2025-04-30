#include <eosio/eosio.hpp>
#include <eosio/call.hpp>

class [[eosio::contract]] call_tests : public eosio::contract{
public:
   using contract::contract;

   [[eosio::action]]
   void basictest() {
      std::vector<char> data{};
      auto rc = eosio::call("calltests"_n, 0, data)();
      // For now, because call dispatcher has not been implemented yet, call should return -1
      eosio::check(rc == -1, "call did not return -1");

      std::vector<char> value(10);
      auto size = eosio::get_call_return_value(value.data(), value.size());
      // call was not executed. return value size should be 0
      eosio::check(size == 0, "return value size is not 0");
   }
};
