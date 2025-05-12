#include "sync_call_callee.hpp"
#include <eosio/print.hpp>

[[eosio::call]]
uint32_t sync_call_callee::getten() {
   return 10;
}

[[eosio::call]]
uint32_t sync_call_callee::getback(uint32_t in) {
   return in;
}

[[eosio::call]]
void sync_call_callee::voidfunc() {
   eosio::print("I am a void function");
}

[[eosio::action, eosio::call]]
uint32_t sync_call_callee::sum(uint32_t a, uint32_t b, uint32_t c) {
   return a + b + c;
}
