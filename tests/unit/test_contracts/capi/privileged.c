#include <eosio/privileged.h>
#include <stdint.h>

void test_privileged( void ) {
   get_resource_limits(0, NULL, NULL, NULL);
   set_resource_limits(0, 0, 0, 0);
   set_proposed_producers(NULL, 0);
   set_proposed_producers_ex(0, NULL, 0);
   is_privileged(0);
   set_privileged(0, 0);
   set_blockchain_parameters_packed(NULL, 0);
   get_blockchain_parameters_packed(NULL, 0);
   preactivate_feature(NULL);
   set_fee_parameters(0, 0, 0, 0);
   config_fee_limits(0, 0, 0);
   set_fee_limits(0, 0, 0);
   get_fee_consumption(0, NULL, NULL);
}
