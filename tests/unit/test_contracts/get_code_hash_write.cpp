#include <eosio/eosio.hpp>
#include <eosio/action.hpp>
#include <eosio/name.hpp>

#include "get_code_hash_table.hpp"

class [[eosio::contract]] get_code_hash_tests : public contract {
public:
   using contract::contract;

   using hash_table = multi_index<name("code.hash"), code_hash>;

   // Write this code's hash to database
   [[eosio::action]]
   void theaction() {
      require_auth(get_self());
      hash_table hashes(get_self(), get_self().value);

      auto hash = get_code_hash(get_self());
      check(hash != checksum256(), "Code hash should not be null");

      hashes.emplace(get_self(), [&hash](auto& t) {
         t.id = 0;
         t.hash = hash;
      });
   }
};

