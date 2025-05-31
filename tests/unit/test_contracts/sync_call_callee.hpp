#include <eosio/call.hpp>
#include <eosio/eosio.hpp>

struct struct1_t {
   int64_t a;
   uint64_t b;
};

struct struct2_t {
   char a;
   bool b;
   int64_t c;
   uint64_t d;
};

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

   // pass in a struct and return it
   [[eosio::call]]
   struct1_t structonly(struct1_t s);

   // pass in two structs and an integer, multiply each field in the struct by
   // the integer, add last two fields of the second struct, and return the result
   [[eosio::call]]
   struct1_t structmix(struct1_t s1, int32_t m, struct2_t s2);

   using getten_func = eosio::call_wrapper<"getten"_n, &sync_call_callee::getten>;
   using getback_func = eosio::call_wrapper<"getback"_n, &sync_call_callee::getback>;
   using voidfunc_func = eosio::call_wrapper<"voidfunc"_n, &sync_call_callee::voidfunc>;
   using sum_func = eosio::call_wrapper<"sum"_n, &sync_call_callee::sum>;
   using structonly_func = eosio::call_wrapper<"structonly"_n, &sync_call_callee::structonly>;
   using structmix_func = eosio::call_wrapper<"structmix"_n, &sync_call_callee::structmix>;

   using void_no_op_success_func = eosio::call_wrapper<"voidfunc"_n, &sync_call_callee::voidfunc, eosio::access_mode::read_write, eosio::support_mode::no_op>;
   using sum_no_op_success_func = eosio::call_wrapper<"sum"_n, &sync_call_callee::sum, eosio::access_mode::read_write, eosio::support_mode::no_op>;
};
