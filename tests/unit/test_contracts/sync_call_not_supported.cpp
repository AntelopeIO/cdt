#include <eosio/eosio.hpp>
#include <eosio/call.hpp>

class [[eosio::contract]] sync_call_not_supported : public eosio::contract{
public:
   using contract::contract;

   // * sync call is not supported as no method is taged by `call`
   // * no_op_if_receiver_no_support_sync_call is set
   [[eosio::action]]
   void noopset() {
      std::vector<char> data{};

      // For now, because sync_call entry point has not been implemented yet and
      // no_op_if_receiver_no_support_sync_call is set to true, call should return -1
      auto rc = eosio::call("caller"_n, data, false /* read_only */, true /* no_op_if_receiver_no_support_sync_call */)();
      eosio::check(rc == -1, "call did not return -1");

      // call was not executed. return value size should be 0
      std::vector<char> value(10);
      auto size = eosio::get_call_return_value(value.data(), value.size());
      eosio::check(size == 0, "return value size is not 0");
   }

   // sync call not supported, no_op_if_receiver_no_support_sync_call not set
   [[eosio::action]]
   void noopnotset() {
      std::vector<char> data{};

      // For now, because sync_call entry point has not been implemented yet and
      // no_op_if_receiver_no_support_sync_call is not set, call should fail
      eosio::call("caller"_n, data)();
   }

   [[eosio::call]]
   uint32_t callee() {
      return 0;
   }
};
