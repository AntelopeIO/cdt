#include "sync_call_callee.hpp"
#include <eosio/print.hpp>

[[eosio::call]]
uint32_t sync_call_callee::return_ten() {
   return 10;
}

[[eosio::call]]
uint32_t sync_call_callee::echo_input(uint32_t in) {
   return in;
}

[[eosio::call]]
void sync_call_callee::void_func() {
   eosio::print("I am a void function");
}

[[eosio::action, eosio::call]]
uint32_t sync_call_callee::sum(uint32_t a, uint32_t b, uint32_t c) {
   return a + b + c;
}

[[eosio::call]]
struct1_t sync_call_callee::pass_single_struct(struct1_t s) {
   return s;
}

[[eosio::call]]
struct1_t sync_call_callee::pass_multi_structs(struct1_t s1, int32_t m, struct2_t s2) {
   return { .a = s1.a * m + s2.c, .b = s1.b * m + s2.d };
}

bool sync_call_callee::issynccall() {
   return is_sync_call();
}
