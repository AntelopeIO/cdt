#include "sync_call_not_supported.hpp"

[[eosio::action]]
void sync_call_not_supported::voidfunc() {
}

[[eosio::action]]
int sync_call_not_supported::intfunc() {
   return 1;
}
