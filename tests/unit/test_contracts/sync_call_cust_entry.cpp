/*
 * This contract is used to verify customized sync_call entry function works.
 * It verifies customized sync_call entry is exported, and `call()` host function
 * can reach it.
 */

#include <eosio/eosio.hpp>
#include <eosio/call.hpp>

namespace eosio {

class [[eosio::contract]] sync_call_cust_entry : public contract {
public:
   using contract::contract;

   [[eosio::action]]
   void cusentrytst1() {
      // Make a sync call to "receiver"_n account, pass function ID in `data`,
      // go to `sync_call()` entry function, and dispatch the call to `get_10()`
      // based on the function ID.
      const std::vector<char> data{ eosio::pack(1) }; // function ID 1
      eosio::call("receiver"_n, 0, data.data(), data.size());

      // Retrieve return value and unpack it
      std::vector<char> return_value(sizeof(uint32_t));
      eosio::get_call_return_value(return_value.data(), return_value.size());

      // Verify it
      eosio::check(eosio::unpack<uint32_t>(return_value) == 10, "return value is not 10");
   }

   [[eosio::action]]
   void cusentrytst2() {
      // Make a sync call to "receiver"_n account, pass function ID in `data`,
      // go to `sync_call()` entry function, and dispatch the call to `get_20()`
      // based on the function ID.
      const std::vector<char> data{ eosio::pack(2) }; // function ID 2
      eosio::call("receiver"_n, 0, data.data(), data.size());

      // Retrieve return value and unpack it
      std::vector<char> return_value(sizeof(uint32_t));
      eosio::get_call_return_value(return_value.data(), return_value.size());

      // Verify it
      eosio::check(eosio::unpack<uint32_t>(return_value) == 20, "return value is not 20");
   }

   // `eosio::call` tag is optional
   uint32_t get_10() {
      return 10;
   }

   [[eosio::call]]
   uint32_t get_20() {
      return 20;
   }
};
} /// namespace eosio

extern "C" {
   [[eosio::wasm_entry]]
   int64_t sync_call(uint64_t sender, uint64_t receiver, uint32_t data_size) {
      eosio::datastream<const char*> ds(nullptr, 0); // for testing, not used
      eosio::sync_call_cust_entry obj(eosio::name{receiver}, eosio::name{receiver}, ds);

      std::vector<char> data(sizeof(uint32_t));
      eosio::get_call_data(data.data(), data.size());
      auto func_id = eosio::unpack<uint32_t>(data);

      // Dispatch sync calls
      uint32_t rv = 0;
      switch (func_id) {
         case 1:  rv = obj.get_10(); break;
         case 2:  rv = obj.get_20(); break;
         default: eosio::check(false, "wrong function ID");
      }

      // set return value
      eosio::set_call_return_value(&rv, sizeof(uint32_t));

      return 0; // return 0 to indicate success
   }
}
