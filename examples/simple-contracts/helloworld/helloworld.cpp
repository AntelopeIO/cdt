#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
using namespace eosio;

CONTRACT enf: public contract {
   public:
      using contract::contract;

      [[eosio::action]] void declare() {
         print("Hello World!\n");    
      }

};

extern "C" {
   [[eosio::wasm_entry]]
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
   }
} 
