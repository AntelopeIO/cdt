/**
 *  @file
 *  @copyright defined in cdt/LICENSE
 */
#pragma once

#include "fixed_bytes.hpp"
#include "varint.hpp"
#include "serialize.hpp"

#include <array>

namespace eosio {


   //using bls_public_key = std::array<char, 48>;
   //using bls_signature = std::array<char, 96>;


   using bls_public_key = std::array<uint8_t, 49>;
   using bls_signature = std::array<uint8_t, 97>;

   namespace internal_use_do_not_use {
      extern "C" {

         __attribute__((eosio_wasm_import))
         bool bls_verify( const char* sig, uint32_t sig_len, const char* dig, uint32_t dig_len, char* pub, uint32_t pub_len);

         __attribute__((eosio_wasm_import))
         int32_t bls_aggregate_pubkeys( const char* pubs, uint32_t pubs_len, const char* agg, uint32_t agg_len);

         __attribute__((eosio_wasm_import))
         int32_t bls_aggregate_sigs( const char* sigs, uint32_t sigs_len, const char* agg, uint32_t agg_len);

         __attribute__((eosio_wasm_import))
         bool bls_aggregate_verify( const char* sig, uint32_t sig_len, const char* digs, uint32_t digs_len, char* pubs, uint32_t pubs_len);

      }

   }

   bool bls_verify(bls_public_key pub, bls_signature sig , std::vector<uint8_t> msg) {

      auto pub_data = eosio::pack(pub);
      auto sig_data = eosio::pack(sig);
      auto msg_data = eosio::pack(msg);

      return internal_use_do_not_use::bls_verify( sig_data.data(), sig_data.size(), msg_data.data(), msg_data.size(), pub_data.data(), pub_data.size() );

   }

   bls_public_key bls_aggregate_pubkeys(std::vector<bls_public_key> pubkeys) {

      auto pubs_data = eosio::pack(pubkeys);

      void* agg_data = alloca(sizeof(bls_public_key));

      int32_t agg_return_size = internal_use_do_not_use::bls_aggregate_pubkeys( pubs_data.data(), pubs_data.size(), reinterpret_cast<char*>(agg_data), sizeof(bls_public_key));

      eosio::datastream< char*> r_agg_data_ds( reinterpret_cast< char*>(agg_data), sizeof(bls_public_key) );

      std::array<uint8_t, sizeof(bls_public_key)> v;

      r_agg_data_ds >> v;

      return v;

   }


   bls_signature bls_aggregate_sigs(std::vector<bls_signature> sigs) {

      auto sigs_data = eosio::pack(sigs);

      void* agg_data = alloca(sizeof(bls_signature));

      int32_t agg_return_size = internal_use_do_not_use::bls_aggregate_sigs( sigs_data.data(), sigs_data.size(), reinterpret_cast<char*>(agg_data), sizeof(bls_signature));

      eosio::datastream< char*> r_agg_data_ds( reinterpret_cast< char*>(agg_data), sizeof(bls_signature) );

      std::array<uint8_t, sizeof(bls_signature)> v;

      r_agg_data_ds >> v;

      return v;

   }

   bool bls_aggregate_verify( std::vector<bls_public_key> pubkeys, std::vector<std::vector<uint8_t>> messages, bls_signature sig ){

      auto pub_data = eosio::pack(pubkeys);
      auto sig_data = eosio::pack(sig);
      auto msg_data = eosio::pack(messages);

      return internal_use_do_not_use::bls_aggregate_verify( sig_data.data(), sig_data.size(), msg_data.data(), msg_data.size(), pub_data.data(), pub_data.size() );

   }

/*   bool bls_verify( const char* sig, uint32_t sig_len, const char* dig, uint32_t dig_len, char* pub, uint32_t pub_len) {
      return internal_use_do_not_use::bls_verify( sig, sig_len, dig, dig_len, pub, pub_len );
   }
*/
}