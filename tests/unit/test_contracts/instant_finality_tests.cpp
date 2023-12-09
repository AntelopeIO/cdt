#include <eosio/eosio.hpp>
#include <eosio/instant_finality.hpp>

class [[eosio::contract]] instant_finality_tests : public eosio::contract{
public:
    using contract::contract;

    [[eosio::action]]
    void setfinalizer(const eosio::abi_finalizer_policy& finalizer_policy) {
        eosio::set_finalizers(finalizer_policy);
    }
};
