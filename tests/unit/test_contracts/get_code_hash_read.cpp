#include <eosio/eosio.hpp>
#include <eosio/action.hpp>
#include <eosio/name.hpp>

#include "get_code_hash_table.hpp"

class [[eosio::contract]] get_code_hash_tests : public contract {
public:
   using contract::contract;

   using hash_table = multi_index<name("code.hash"), code_hash>;

   // Read the old code's hash from database and verify new code's hash differs
   [[eosio::action]]
   void theaction() {
      require_auth(get_self());
      hash_table hashes(get_self(), get_self().value);

      auto hash = get_code_hash(get_self());
      check(hash != checksum256(), "Code hash should not be null");

      auto record = hashes.get(0, "Unable to find recorded hash");
      check(hash != record.hash, "Code hash has not changed");
      eosio::print("Old hash: ", record.hash, "; new hash: ", hash);
   }
};

