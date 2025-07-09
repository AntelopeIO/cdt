#include <eosio/call.hpp>
#include <eosio/print.hpp>
#include <eosio/eosio.hpp>

class [[eosio::contract]] sync_call_single_func : public eosio::contract{
public:
   using contract::contract;

   [[eosio::call]]
   uint32_t getten() {
      return 10;
   }

   // Do NOT add any more functions tagged as `eosio::call` in this contract.
   // It is used to test a contract having only one function tagged as `eosio::call`.
};
