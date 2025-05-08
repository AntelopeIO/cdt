#include <eosio/eosio.hpp>
#include <eosio/call.hpp>

class [[eosio::contract]] sync_call_callee : public eosio::contract{
public:
   using contract::contract;

   [[eosio::call]]
   uint32_t getten() {
      return 10;
   }

   [[eosio::call]]
   uint32_t getback(uint32_t in) {
      return in;
   }

   [[eosio::action]]
   void dummy() {
      return;
   }
};
