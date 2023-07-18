#pragma once

#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/singleton.hpp>
#include <eosio/multi_index.hpp>



struct [[eosio::table]] out_of_class {
    uint64_t id;
    uint64_t primary_key() const { return id; }
};
typedef eosio::multi_index<"mi.config55"_n, out_of_class> out_of_class_index;
using uout_of_class_index = eosio::multi_index<"mi.config551"_n, out_of_class>;

typedef eosio::singleton<"smpl.conf55"_n, eosio::name> smpl_config55;
typedef eosio::singleton<"config55"_n, out_of_class> config55;
typedef smpl_config55 smpl_config551;
typedef config55 config551;
using  smpl_conf551 = eosio::singleton<"smpl.conf551"_n, eosio::name>;
using  config552 = eosio::singleton<"config552"_n, out_of_class>;
using smpl_conf552 = smpl_conf551;
using config553 = config551;

class [[eosio::contract("singleton_contract_simple2")]] singleton_contract_simple2 : public eosio::contract {
    public:
        using eosio::contract::contract;
        
    [[eosio::action]]
        void whatever() {};
    
    struct [[eosio::table]] inside_class {
        uint64_t id;
        uint64_t primary_key() const { return id; }
    };
    typedef eosio::singleton<"smpl.conf552"_n, eosio::name> smpl_conf552;
    typedef eosio::singleton<"config552"_n, inside_class> config552;
    typedef smpl_conf552 smpl_conf553;
    typedef config552 config553;
    using smpl_conf554 = eosio::singleton<"smpl.conf554"_n, eosio::name>;
    using config554 = eosio::singleton<"config554"_n, inside_class>;
    using smpl_conf555 = smpl_conf554;
    using config555 = config554;



    typedef eosio::multi_index<"mi.config553"_n, inside_class> inside_class_index;
    using uinside_class_index = eosio::multi_index<"mi.config554"_n, inside_class>;
};
