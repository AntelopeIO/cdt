#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>
#include <eosio/crypto_ext.hpp>

using namespace eosio;

class [[eosio::contract]] crypto_primitives_tests : public contract{
   public:
      using contract::contract;

      [[eosio::action]]
      void sha3test(std::string val, eosio::checksum256 sha3_dg) {
         auto hash = eosio::sha3(val.c_str(), val.size());

         eosio::check(hash == sha3_dg, "SHA3 doesn't match expected");
         eosio::assert_sha3(val.c_str(), val.size(), sha3_dg);
      }

      [[eosio::action]]
      void keccaktest(std::string val, eosio::checksum256 sha3_dg) {
         auto hash = eosio::keccak(val.c_str(), val.size());

         eosio::check(hash == sha3_dg, "Keccak doesn't match expected");
         eosio::assert_keccak(val.c_str(), val.size(), sha3_dg);
      }

};
