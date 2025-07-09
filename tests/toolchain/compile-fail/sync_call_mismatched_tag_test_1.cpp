#include <eosio/call.hpp>
#include <eosio/eosio.hpp>

// Test the validation that action and call tags cannot be mixed.

class [[eosio::contract]] invalid_call_tags : public eosio::contract{
public:
   using contract::contract;

   [[eosio::action, eosio::call]]
   uint32_t act1() {
      return 0;
   }

   [[eosio::call, eosio::action]]
   uint32_t act2() {
      return 0;
   }
};
