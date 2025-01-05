#include <eosio/eosio.hpp>
using namespace eosio;

CONTRACT enf: public contract {
   public:
      using contract::contract;

      [[eosio::action]] void declare() {
         print_f("Hello World!\n"); 
      }
};

EOSIO_DISPATCH(enf, (declare))

//extern "C" {
//   [[eosio::wasm_entry]]
//   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
//      print_f("Hello from Apply\n");
//   }
//} 
