#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/singleton.hpp>
 
using namespace eosio;
 
class [[eosio::contract("singleton_contract_simple")]] singleton_contract_simple : public contract {
    public:
        using contract::contract;
        
    [[eosio::action]]
        void whatever() {};
    
    typedef eosio::singleton<"smpl.config"_n, name> config;
};
