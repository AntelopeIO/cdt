#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>

using namespace eosio;

struct [[eosio::table]] data1 {

   name           someval;

} data1_row;

using data1table = eosio::singleton<"data1"_n, data1>;

class [[eosio::contract("singleton_other_contract")]] singleton_other_contract : public contract {
   public:
      using contract::contract;
      
      struct [[eosio::table, eosio::contract("othercontract")]] data2 {

         name           someval;

      } data2_row;

      using data2table = eosio::singleton<"data2"_n, data2>;

      [[eosio::action]]
         void whatever() {};


};
