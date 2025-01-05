#include <eosio/eosio.hpp>
using namespace eosio;

CONTRACT enf: public contract {
   public:
      using contract::contract;

      [[eosio::action]] uint64_t readnumber(){
         return 42;
      }
};
EOSIO_DISPATCH(enf, (readnumber))
