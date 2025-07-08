#include <eosio/eosio.hpp>
#include <eosio/call.hpp>

// Because this contract does not tag any functions by `call` attribute,
// `sync_call` entry point is not generated.
// Any sync calls to this contract will return a status indicating
// sync calls are not supported by the receiver.
class [[eosio::contract]] sync_call_not_supported : public eosio::contract{
public:
   using contract::contract;

   [[eosio::action]]
   void voidfunc();

   [[eosio::action]]
   int intfunc();

   using void_func = eosio::call_wrapper<"voidfunc"_i, &sync_call_not_supported::voidfunc>;  // default behavior: abort when called
   using void_no_op_func = eosio::call_wrapper<"voidfunc"_i, &sync_call_not_supported::voidfunc, eosio::access_mode::read_write, eosio::support_mode::no_op>; // no op when called
   
   using int_func = eosio::call_wrapper<"intfunc"_i, &sync_call_not_supported::intfunc>;  // default behavior: abort when called
   using int_no_op_func = eosio::call_wrapper<"intfunc"_i, &sync_call_not_supported::intfunc, eosio::access_mode::read_write, eosio::support_mode::no_op>; // no op when called
};
