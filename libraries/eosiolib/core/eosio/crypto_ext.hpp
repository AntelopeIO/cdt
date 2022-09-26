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

   namespace internal_use_do_not_use {
      extern "C" {

         struct __attribute__((aligned (16))) capi_checksum256_ext { uint8_t hash[32]; };

         __attribute__((eosio_wasm_import))
         int32_t alt_bn128_add( const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* result, uint32_t result_len);

         __attribute__((eosio_wasm_import))
         int32_t alt_bn128_mul( const char* g1, uint32_t g1_len, const char* scalar, uint32_t scalar_len, char* result, uint32_t result_len);

         __attribute__((eosio_wasm_import))
         int32_t alt_bn128_pair( const char* pairs, uint32_t pairs_len);

         __attribute__((eosio_wasm_import))
         int32_t mod_exp( const char* base, uint32_t base_len, const char* exp, uint32_t exp_len, const char* mod, uint32_t mod_len, char* result, uint32_t result_len);

         __attribute__((eosio_wasm_import))
         int32_t blake2_f( uint32_t rounds, const char* state, uint32_t state_len, const char* msg, uint32_t msg_len, const char* t0_offset, uint32_t t0_len, const char* t1_offset, uint32_t t1_len, int32_t final, char* result, uint32_t result_len);

         __attribute__((eosio_wasm_import))
         int32_t k1_recover( const char* sig, uint32_t sig_len, const char* dig, uint32_t dig_len, char* pub, uint32_t pub_len);

         __attribute__((eosio_wasm_import))
         void sha3( const char* data, uint32_t data_len, char* hash, uint32_t hash_len, int32_t keccak );
      }

      static inline auto sha3_helper(const char* data, uint32_t length, bool keccak) {
         internal_use_do_not_use::capi_checksum256_ext hash;
         internal_use_do_not_use::sha3( data, length, (char*)&hash, sizeof(hash), keccak);
         eosio::checksum256 dg;
         eosio::datastream<uint8_t*> ds = {&hash.hash[0], sizeof(hash)};
         ds >> dg;
         return dg;
      }
   }
      
   /**
    *  @defgroup crypto Crypto
    *  @ingroup core
    *  @brief Defines API for calculating and checking hashes which
    *  require activating crypto protocol feature
    */

   /**
    *  Addition operation on the elliptic curve `alt_bn128` 
    *
    *  @ingroup crypto
    *  @param op1 - operand 1
    *  @param op1_len - size of operand 1
    *  @param op2 - operand 2
    *  @param op2_len - size of operand 2
    *  @param result - result of the addition operation
    *  @param result_len - size of result
    *  @return -1 if there is an error otherwise 0
    */
   inline int32_t alt_bn128_add( const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* result, uint32_t result_len ) {
      return internal_use_do_not_use::alt_bn128_add( op1, op1_len, op2, op2_len, result, result_len);
   }
   
   /**
    *  Scalar multiplication operation on the elliptic curve `alt_bn128` 
    *
    *  @ingroup crypto
    *  @param g1 - G1 point
    *  @param g1_len - size of G1 point
    *  @param scalar - scalar factor
    *  @param scalar_len - size of scalar
    *  @param result - result of the scalar multiplication operation
    *  @param result_len - size of result
    *  @return -1 if there is an error otherwise 0
    */
   inline int32_t alt_bn128_mul( const char* g1, uint32_t g1_len, const char* scalar, uint32_t scalar_len, char* result, uint32_t result_len ) {
      return internal_use_do_not_use::alt_bn128_mul( g1, g1_len, scalar, scalar_len, result, result_len );
   }
   
   /**
    *  Optimal-Ate pairing check elliptic curve `alt_bn128` 
    *
    *  @ingroup crypto
    *  @param pairs - g1 and g2 pairs
    *  @param pairs_len - size of pairs
    *  @param result - result of the addition operation
    *  @return -1 if there is an error, 1 if false and 0 if true and successful
    */
   inline int32_t alt_bn128_pair( const char* pairs, uint32_t pairs_len ) {
      return internal_use_do_not_use::alt_bn128_pair( pairs, pairs_len );
   }
   
   /**
    *  Big integer modular exponentiation
    *  returns an output ( BASE^EXP ) % MOD
    *
    *  @ingroup crypto
    *  @param base - base of the exponentiation (BASE)
    *  @param base_len - size of base
    *  @param exp - exponent to raise to that power (EXP)
    *  @param exp_len - size of exp
    *  @param mod - modulus (MOD)
    *  @param mod_len - size of mod
    *  @param result - result of the modular exponentiation
    *  @param result_len - size of result
    *  @return -1 if there is an error otherwise 0
    */

   inline int32_t mod_exp( const char* base, uint32_t base_len, const char* exp, uint32_t exp_len, const char* mod, uint32_t mod_len, char* result, uint32_t result_len ) {
      return internal_use_do_not_use::mod_exp( base, base_len, exp, exp_len, mod, mod_len, result, result_len);
   }

   /**
    *  BLAKE2 compression function "F"
    *  https://eips.ethereum.org/EIPS/eip-152
    *
    *  @ingroup crypto
    *  @param rounds - the number of rounds
    *  @param state - state vector
    *  @param state_len - size of state vector
    *  @param msg - message block vector
    *  @param msg_len - size of message block vector
    *  @param t0_offset - offset counters 
    *  @param t0_len - size of t0_offset
    *  @param t1_offset - offset counters 
    *  @param t1_len - size of t1_offset
    *  @param final - final block flag
    *  @param result - the result of the compression
    *  @param result_len - size of result 
    *  @return -1 if there is an error otherwise 0
    */
   int32_t blake2_f( uint32_t rounds, const char* state, uint32_t state_len, const char* msg, uint32_t msg_len, 
                     const char* t0_offset, uint32_t t0_len, const char* t1_offset, uint32_t t1_len, int32_t final, char* result, uint32_t result_len) {
      return internal_use_do_not_use::blake2_f( rounds, state, state_len, msg, msg_len, t0_offset, t0_len, t1_offset, t1_len, final, result, result_len);
   }
   
   /**
    *  Hashes `data` using `sha3`
    *
    *  @param data - data you want to hash
    *  @param length - size of data
    *  @param keccak - whether to use `keccak` or NIST variant; keccak = 1 and NIST == 0
    *  @return eosio::checksum256 - Computed digest
    *
    */

   /**
    *  Hashes `data` using SHA3 NIST.
    *
    *  @ingroup crypto
    *  @param data - Data you want to hash
    *  @param length - Data length
    *  @return eosio::checksum256 - Computed digest
    */
   inline eosio::checksum256 sha3(const char* data, uint32_t length) {
      return internal_use_do_not_use::sha3_helper(data, length, false);
   }

   /**
    *  Tests if the SHA3 hash generated from data matches the provided digest.
    *
    *  @ingroup crypto
    *  @param data - Data you want to hash
    *  @param length - Data length
    *  @param hash - digest to compare to
    *  @note !This method is not optimized away during replay
    */
   inline void assert_sha3(const char* data, uint32_t length, const eosio::checksum256& hash) {
      const auto& res = internal_use_do_not_use::sha3_helper(data, length, false);
      check( hash == res, "SHA3 hash of `data` does not match given `hash`");
   }

   /**
    *  Hashes `data` using SHA3 Keccak.
    *
    *  @ingroup crypto
    *  @param data - Data you want to hash
    *  @param length - Data length
    *  @return eosio::checksum256 - Computed digest
    */
   inline eosio::checksum256 keccak(const char* data, uint32_t length) {
      return internal_use_do_not_use::sha3_helper(data, length, true);
   }

   /**
    *  Tests if the SHA3 keccak hash generated from data matches the provided digest.
    *
    *  @ingroup crypto
    *  @param data - Data you want to hash
    *  @param length - Data length
    *  @param hash - digest to compare to
    *  @note !This method is not optimized away during replay
    */
   inline void assert_keccak(const char* data, uint32_t length, const eosio::checksum256& hash) {
      const auto& res = internal_use_do_not_use::sha3_helper(data, length, true);
      check( hash == res, "Keccak hash of `data` does not match given `hash`");
   }

   /**
    *  Calculates the uncompressed public key used for a given signature on a given digest.
    *
    *  @ingroup crypto
    *  @param sig - signature.
    *  @param sig_len - size of signature
    *  @param dig - digest of the message that was signed.
    *  @param dig_len - size of digest
    *  @param pub - public key result
    *  @param pub_len - size of public key result
    *
    *  @return -1 if there was an error 0 otherwise.
   */
   inline int32_t k1_recover( const char* sig, uint32_t sig_len, const char* dig, uint32_t dig_len, char* pub, uint32_t pub_len ) {
      return internal_use_do_not_use::k1_recover( sig, sig_len, dig, dig_len, pub, pub_len );
   }
}
