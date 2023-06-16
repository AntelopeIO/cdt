#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <array>

using std::array;
using namespace eosio;

class[[eosio::contract("using_uint128_test")]] using_uint128_test : public contract
{
public:
   using contract::contract;
   uint128_t my_number;

   [[eosio::action]] void hi(name user) {
      require_auth(user);
      print("Hello, ", user);
   }

   [[eosio::action]] void printnum(uint128_t input_number) {
      my_number = input_number + input_number;
      print("Number = ", my_number);
   }

   struct [[eosio::table]] greeting {
      uint128_t id;
      array<int, 32> t;
      uint64_t primary_key() const { return id; }
   };
   typedef multi_index<"greeting"_n, greeting> greeting_index;
};
