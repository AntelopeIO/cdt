#include <eosio/call.hpp>
#include <eosio/print.hpp>
#include <eosio/eosio.hpp>

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

   [[eosio::call]]
   void voidfunc() {
      eosio::print("I am a void function");
   }

   [[eosio::call]]
   uint32_t sum(uint32_t in1, uint32_t in2, uint32_t in3) {
      return in1 + in2 + in3;
   }

   [[eosio::action]]
   void dummy() {
      return;
   }
};
