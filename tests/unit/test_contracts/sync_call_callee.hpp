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
   uint32_t return_ten();

   [[eosio::call]]
   uint32_t echo_input(uint32_t in);

   [[eosio::call]]
   void void_func();

   [[eosio::action, eosio::call]]
   uint32_t sum(uint32_t a, uint32_t b, uint32_t c);

   // pass in a struct and return it
   [[eosio::call]]
   struct1_t pass_single_struct(struct1_t s);

   // pass in two structs and an integer, multiply each field in the struct by
   // the integer, add last two fields of the second struct, and return the result
   [[eosio::call]]
   struct1_t pass_multi_structs(struct1_t s1, int32_t m, struct2_t s2);

   using return_ten_func = eosio::call_wrapper<"return_ten"_i, &sync_call_callee::return_ten>;
   using echo_input_func = eosio::call_wrapper<"echo_input"_i, &sync_call_callee::echo_input>;
   using void_func_func = eosio::call_wrapper<"void_func"_i, &sync_call_callee::void_func>;
   using sum_func = eosio::call_wrapper<"sum"_i, &sync_call_callee::sum>;
   using pass_single_struct_func = eosio::call_wrapper<"pass_single_struct"_i, &sync_call_callee::pass_single_struct>;
   using pass_multi_structs_func = eosio::call_wrapper<"pass_multi_structs"_i, &sync_call_callee::pass_multi_structs>;

   using void_no_op_success_func = eosio::call_wrapper<"void_func"_i, &sync_call_callee::void_func, eosio::access_mode::read_write, eosio::support_mode::no_op>;
   using sum_no_op_success_func = eosio::call_wrapper<"sum"_i, &sync_call_callee::sum, eosio::access_mode::read_write, eosio::support_mode::no_op>;
};
