#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <array>

using std::array;
using namespace eosio;

class[[eosio::contract("using_uint256_test")]] using_uint256_test : public contract
{
public:
   using contract::contract;
   uint256_t my_number;

   [[eosio::action]] void hi(name user) {
      require_auth(user);
      print("Hello, ", user);
   }

   [[eosio::action]] void printnum(uint256_t input_number) {
      my_number = input_number + input_number;
      print("Number = ", my_number);
   }

};
