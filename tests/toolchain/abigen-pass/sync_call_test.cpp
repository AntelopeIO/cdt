#include <eosio/call.hpp>
#include <eosio/eosio.hpp>

class [[eosio::contract]] sync_call_test : public eosio::contract {
public:
   using contract::contract;

   [[eosio::call]]
   uint32_t noparam() {
      return 10;
   }
   
   [[eosio::call]]
   uint32_t withparam(uint32_t in) {
      return in;
   }
   
   [[eosio::call]]
   void voidfunc() {
      int i = 10;
   }
   
   [[eosio::call]]
   uint32_t sum(uint32_t a, uint32_t b, uint32_t c) {
      return a + b + c;
   }
};
