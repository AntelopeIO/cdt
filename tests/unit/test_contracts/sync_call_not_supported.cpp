#include <eosio/eosio.hpp>
#include <eosio/call.hpp>

// This contract does not tag any function by `call` attribute.
// No `sync_call` entry point is generated.
// Sync calls to any functions in this contract will fail.
class [[eosio::contract]] sync_call_not_supported : public eosio::contract{
public:
   using contract::contract;

   [[eosio::action]]
   void dummy() {
   }
   using dummy_func = eosio::call_wrapper<"dummy"_n, &sync_call_not_supported::dummy>;
   using dummy_no_op_func = eosio::call_wrapper<"dummy"_n, &sync_call_not_supported::dummy, eosio::execution_mode::read_write, eosio::on_call_not_supported_mode::no_op>;

   // request no-op
   [[eosio::action]]
   void noopifnot() {
      dummy_no_op_func dummy_f{"caller"_n};
      dummy_f();
   }

   // request abort
   [[eosio::action]]
   void abortifnot() {
      dummy_func dummy_f{"caller"_n};  // default is abort
      dummy_f();
   }
};
