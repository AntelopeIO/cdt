//
// Created by Eric Passmore on 12/17/24.
//

#include <eosio/tester.hpp>
#include <eosio/string.hpp>

using eosio::string;

EOSIO_TEST_BEGIN(inside_test)
     const string eostr1{"Hello! abcdef"};
     eostr1.print();
EOSIO_TEST_END
