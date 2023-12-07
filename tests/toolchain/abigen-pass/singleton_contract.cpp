#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/singleton.hpp>

#include "exclude_from_abi.hpp"
 
using namespace eosio;

struct [[eosio::table]] out_of_class2 {
    uint64_t id;
    uint64_t primary_key() const { return id; }
};
typedef eosio::multi_index<"mi.config5"_n, out_of_class2> out_of_class_index51;
using uout_of_class_index51 = eosio::multi_index<"mi.config51"_n, out_of_class2>;

struct [[eosio::table, eosio::contract("singleton_contract")]] out_of_class3 {
    uint64_t id;
    uint64_t primary_key() const { return id; }
};
typedef eosio::multi_index<"mi.config52"_n, out_of_class3> out_of_class_index52;

typedef eosio::singleton<"smpl.conf5"_n, eosio::name> smpl_config5;
typedef eosio::singleton<"config5"_n, out_of_class2> config5;
typedef smpl_config5 smpl_config51;
typedef config5 config51;
using  smpl_conf51 = eosio::singleton<"smpl.conf51"_n, eosio::name>;
using  config52 = eosio::singleton<"config52"_n, out_of_class2>;
using smpl_conf52 = smpl_conf51;
using config53 = config51;

class [[eosio::contract("singleton_contract")]] singleton_contract : public contract {
    public:
        using contract::contract;
        
    [[eosio::action]]
        void whatever() {};
 
    struct [[eosio::table]] tbl_config {
        uint64_t y;
        uint64_t x;
    };
    
    typedef eosio::singleton<"config"_n, tbl_config>    config;
    typedef eosio::singleton<"smpl.config"_n, name>     smpl_config;
    using smpl_config2 = smpl_config5;
    typedef config551 config2; //from exclude_from_abi.hpp
};
