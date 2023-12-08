#include <eosio/eosio.hpp>
#include <eosio/finalizer_policy.hpp>

class [[eosio::contract]] set_finalizers_tests : public eosio::contract{
public:
    using contract::contract;

    [[eosio::action]]
    void setfinal(const eosio::abi_finalizer_policy& finalizer_policy) {
        eosio::set_finalizers(finalizer_policy);
    }
};
