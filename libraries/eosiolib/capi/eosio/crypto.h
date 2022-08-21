/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once
#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup crypto Crypto
 *  @brief Defines %C API for calculating and checking hash
 *  @{
 */

/**
 *  Tests if the sha256 hash generated from data matches the provided checksum.
 *
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - `capi_checksum256*` hash to compare to
 *
 *  @pre **assert256 hash** of `data` equals provided `hash` parameter.
 *  @post Executes next statement. If was not `true`, hard return.
 *
 *  @note This method is optimized to a NO-OP when in fast evaluation mode.
 *
 *  Example:
 *
 *  @code
 *  checksum hash;
 *  char data;
 *  uint32_t length;
 *  assert_sha256( data, length, hash )
 *  //If the sha256 hash generated from data does not equal provided hash, anything below will never fire.
 *  eosio::print("sha256 hash generated from data equals provided hash");
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void assert_sha256( const char* data, uint32_t length, const struct capi_checksum256* hash );

/**
 *  Tests if the sha1 hash generated from data matches the provided checksum.
 *
 *  @note This method is optimized to a NO-OP when in fast evaluation mode.
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - `capi_checksum160*` hash to compare to
 *
 *  @pre **sha1 hash** of `data` equals provided `hash` parameter.
 *  @post Executes next statement. If was not `true`, hard return.
 *
 *  Example:
*
 *  @code
 *  checksum hash;
 *  char data;
 *  uint32_t length;
 *  assert_sha1( data, length, hash )
 *  //If the sha1 hash generated from data does not equal provided hash, anything below will never fire.
 *  eosio::print("sha1 hash generated from data equals provided hash");
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void assert_sha1( const char* data, uint32_t length, const struct capi_checksum160* hash );

/**
 *  Tests if the sha512 hash generated from data matches the provided checksum.
 *
 *  @note This method is optimized to a NO-OP when in fast evaluation mode.
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - `capi_checksum512*` hash to compare to
 *
 *  @pre **assert512 hash** of `data` equals provided `hash` parameter.
 *  @post Executes next statement. If was not `true`, hard return.
 *
 *  Example:
*
 *  @code
 *  checksum hash;
 *  char data;
 *  uint32_t length;
 *  assert_sha512( data, length, hash )
 *  //If the sha512 hash generated from data does not equal provided hash, anything below will never fire.
 *  eosio::print("sha512 hash generated from data equals provided hash");
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void assert_sha512( const char* data, uint32_t length, const struct capi_checksum512* hash );

/**
 *  Tests if the ripemod160 hash generated from data matches the provided checksum.
 *
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - `capi_checksum160*` hash to compare to
 *
 *  @pre **assert160 hash** of `data` equals provided `hash` parameter.
 *  @post Executes next statement. If was not `true`, hard return.
 *
 *  Example:
*
 *  @code
 *  checksum hash;
 *  char data;
 *  uint32_t length;
 *  assert_ripemod160( data, length, hash )
 *  //If the ripemod160 hash generated from data does not equal provided hash, anything below will never fire.
 *  eosio::print("ripemod160 hash generated from data equals provided hash");
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void assert_ripemd160( const char* data, uint32_t length, const struct capi_checksum160* hash );

/**
 *  Hashes `data` using `sha256` and stores result in memory pointed to by hash.
 *
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - Hash pointer
 *
 *  Example:
*
 *  @code
 *  checksum calc_hash;
 *  sha256( data, length, &calc_hash );
 *  eos_assert( calc_hash == hash, "invalid hash" );
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void sha256( const char* data, uint32_t length, struct capi_checksum256* hash );

/**
 *  Hashes `data` using `sha1` and stores result in memory pointed to by hash.
 *
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - Hash pointer
 *
 *  Example:
*
 *  @code
 *  checksum calc_hash;
 *  sha1( data, length, &calc_hash );
 *  eos_assert( calc_hash == hash, "invalid hash" );
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void sha1( const char* data, uint32_t length, struct capi_checksum160* hash );

/**
 *  Hashes `data` using `sha512` and stores result in memory pointed to by hash.
 *
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - Hash pointer
 *
 *  Example:
*
 *  @code
 *  checksum calc_hash;
 *  sha512( data, length, &calc_hash );
 *  eos_assert( calc_hash == hash, "invalid hash" );
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void sha512( const char* data, uint32_t length, struct capi_checksum512* hash );

/**
 *  Hashes `data` using `sha3` and stores result in memory pointed to by hash.
 *
 *  @param data - data you want to hash
 *  @param data_len - size of data
 *  @param hash - hash result
 *  @param hash_len - size of hash result
 *  @param keccak - whether to use `keccak` or NIST variant; keccak = 1 and NIST == 0
 *
 */
__attribute__((eosio_wasm_import))
void sha3( const char* data, uint32_t data_len, char* hash, uint32_t hash_len, int32_t keccak );

/**
 *  Hashes `data` using `ripemod160` and stores result in memory pointed to by hash.
 *
 *  @param data - Data you want to hash
 *  @param data_len - Data length
 *  @param hash - Hash pointer
 *
 *  Example:
*
 *  @code
 *  checksum calc_hash;
 *  ripemod160( data, length, &calc_hash );
 *  eos_assert( calc_hash == hash, "invalid hash" );
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void ripemd160( const char* data, uint32_t length, struct capi_checksum160* hash );

/**
 *  BLAKE2 compression function "F"
 *  https://eips.ethereum.org/EIPS/eip-152
 *
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
__attribute__((eosio_wasm_import))
int32_t blake2_f( uint32_t rounds, const char* state, uint32_t state_len, const char* msg, uint32_t msg_len, 
                  const char* t0_offset, uint32_t t0_len, const char* t1_offset, uint32_t t1_len, int32_t final, char* result, uint32_t result_len);

/**
 *  Calculates the public key used for a given signature and hash used to create a message.
 *
 *  @param digest - Hash used to create a message
 *  @param sig - Signature
 *  @param siglen - Signature length
 *  @param pub - Public key
 *  @param publen - Public key length
*   @return int - number of bytes written to pub
 *
 *  Example:
*
 *  @code
 *  @endcode
 */
__attribute__((eosio_wasm_import))
int recover_key( const struct capi_checksum256* digest, const char* sig, size_t siglen, char* pub, size_t publen );

/**
 *  Tests a given public key with the generated key from digest and the signature.
 *
 *  @param digest - What the key will be generated from
 *  @param sig - Signature
 *  @param siglen - Signature length
 *  @param pub - Public key
 *  @param publen - Public key length
 *
 *  @pre **assert recovery key** of `pub` equals the key generated from the `digest` parameter
 *  @post Executes next statement. If was not `true`, hard return.
 *
 *  Example:
*
 *  @code
 *  checksum digest;
 *  char sig;
 *  size_t siglen;
 *  char pub;
 *  size_t publen;
 *  assert_recover_key( digest, sig, siglen, pub, publen )
 *  // If the given public key does not match with the generated key from digest and the signature, anything below will never fire.
 *  eosio::print("pub key matches the pub key generated from digest");
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void assert_recover_key( const struct capi_checksum256* digest, const char* sig, size_t siglen, const char* pub, size_t publen );

/**
 * Calculates the uncompressed public key used for a given signature on a given digest.
 *
 * @param sig - signature.
 * @param sig_len - size of signature
 * @param dig - digest of the message that was signed.
 * @param dig_len - size of digest
 * @param pub - public key result
 * @param pub_len - size of public key result
 *
 * @return -1 if there was an error 0 otherwise.
*/
__attribute__((eosio_wasm_import))
int32_t k1_recover( const char* sig, uint32_t sig_len, const char* dig, uint32_t dig_len, char* pub, uint32_t pub_len);

/**
 *  Addition operation on the elliptic curve `alt_bn128` 
 *
 *  @param op1 - operand 1
 *  @param op1_len - size of operand 1
 *  @param op2 - operand 2
 *  @param op2_len - size of operand 2
 *  @param result - result of the addition operation
 *  @param result_len - size of result
 *  @return -1 if there is an error otherwise 0
 */
__attribute__((eosio_wasm_import))
int32_t alt_bn128_add( const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* result, uint32_t result_len);

/**
 *  Scalar multiplication operation on the elliptic curve `alt_bn128` 
 *
 *  @param g1 - G1 point
 *  @param g1_len - size of G1 point
 *  @param scalar - scalar factor
 *  @param scalar_len - size of scalar
 *  @param result - result of the scalar multiplication operation
 *  @param result_len - size of result
 *  @return -1 if there is an error otherwise 0
 */
__attribute__((eosio_wasm_import))
int32_t alt_bn128_mul( const char* g1, uint32_t g1_len, const char* scalar, uint32_t scalar_len, char* result, uint32_t result_len);

/**
 *  Optimal-Ate pairing check elliptic curve `alt_bn128` 
 *
 *  @param pairs - g1 and g2 pairs
 *  @param pairs_len - size of pairs
 *  @param result - result of the addition operation
 *  @return -1 if there is an error, 1 if false and 0 if true and successful
 */
__attribute__((eosio_wasm_import))
int32_t alt_bn128_pair( const char* pairs, uint32_t pairs_len);

/**
 *  Big integer modular exponentiation
 *  returns an output ( BASE^EXP ) % MOD
 *
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
__attribute__((eosio_wasm_import))
int32_t mod_exp( const char* base, uint32_t base_len, const char* exp, uint32_t exp_len, const char* mod, uint32_t mod_len, char* result, uint32_t result_len);

#ifdef __cplusplus
}
#endif
/// @}
