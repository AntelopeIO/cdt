#include <eosio/eosio.hpp>

using namespace eosio; 

CONTRACT enf: public contract {
   public:
      using contract::contract;

      [[eosio::action]] void fixtransfer() {
            action(
                permission_level{get_self(), name("active")},
                name("eosio.token"),
                name("transfer"),
                std::make_tuple(get_self(), name("webaccount1"), "1.000 EOS", "llvm16 fixed trans")
            ).send();
      }
};

EOSIO_DISPATCH(enf, (fixtransfer))

