#include <eosio/call.hpp>
#include <eosio/eosio.hpp>

// Test the validation that action and call tags cannot be mixed.

class [[eosio::contract]] invalid_call_tags : public eosio::contract{
public:
   using contract::contract;

   [[eosio::action]]
   uint32_t act();
};

[[eosio::call]]
uint32_t invalid_call_tags::act() {
   return 0;
}
