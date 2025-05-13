#include <eosio/call.hpp>
#include <eosio/eosio.hpp>

class [[eosio::contract]] sync_call_callee : public eosio::contract{
public:
   using contract::contract;

   [[eosio::call]]
   uint32_t getten();

   [[eosio::call]]
   uint32_t getback(uint32_t in);

   [[eosio::call]]
   void voidfunc();

   [[eosio::action, eosio::call]]
   uint32_t sum(uint32_t a, uint32_t b, uint32_t c);

   using getten_func = eosio::call_wrapper<"getten"_n, &sync_call_callee::getten>;
   using getback_func = eosio::call_wrapper<"getback"_n, &sync_call_callee::getback>;
   using voidfunc_func = eosio::call_wrapper<"voidfunc"_n, &sync_call_callee::voidfunc>;
   using sum_func = eosio::call_wrapper<"sum"_n, &sync_call_callee::sum>;
};
