/**
 *  @file
 *  @copyright defined in cdt/LICENSE
 */
#pragma once
#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((eosio_wasm_import))
bool bls_verify( const char* sig, uint32_t sig_len, const char* dig, uint32_t dig_len, char* pub, uint32_t pub_len);

__attribute__((eosio_wasm_import))
int32_t bls_aggregate_pubkeys( const char* pubs, uint32_t pubs_len, const char* agg, uint32_t agg_len);

__attribute__((eosio_wasm_import))
int32_t bls_aggregate_sigs( const char* sigs, uint32_t sigs_len, const char* agg, uint32_t agg_len);

__attribute__((eosio_wasm_import))
bool bls_aggregate_verify( const char* sig, uint32_t sig_len, const char* digs, uint32_t digs_len, char* pubs, uint32_t pubs_len);

#ifdef __cplusplus
}
#endif
/// @}