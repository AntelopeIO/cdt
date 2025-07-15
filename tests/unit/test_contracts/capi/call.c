#include <eosio/call.h>
#include <stddef.h>

void test_call( void ) {
   call(0, 0, NULL, 0);
   get_call_return_value(NULL, 0);
   get_call_data(NULL, 0);
   set_call_return_value(NULL, 0);
}
