#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/singleton.hpp>
 
using namespace eosio;
 
class [[eosio::contract("singleton_contract")]] singleton_contract : public contract {
    public:
        using contract::contract;
        
    [[eosio::action]]
        void whatever() {};
 
    struct [[eosio::table]] tbl_config {
        uint64_t y;
        uint64_t x;
    };
    
    typedef eosio::singleton<"config"_n, tbl_config> config;
};
