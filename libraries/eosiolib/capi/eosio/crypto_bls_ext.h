#pragma once
#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((eosio_wasm_import))
int32_t bls_g1_add(const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* res, uint32_t res_len);

__attribute__((eosio_wasm_import))
int32_t bls_g2_add(const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* res, uint32_t res_len);

__attribute__((eosio_wasm_import))
int32_t bls_g1_weighted_sum(const char* points, uint32_t points_len, const char* scalars, uint32_t scalars_len, uint32_t n, char* res, uint32_t res_len);

__attribute__((eosio_wasm_import))
int32_t bls_g2_weighted_sum(const char* points, uint32_t points_len, const char* scalars, uint32_t scalars_len, uint32_t n, char* res, uint32_t res_len);

__attribute__((eosio_wasm_import))
int32_t bls_pairing(const char* g1_points, uint32_t g1_points_len, const char* g2_points, uint32_t g2_points_len, uint32_t n, char* res, uint32_t res_len);

__attribute__((eosio_wasm_import))
int32_t bls_g1_map(const char* e, uint32_t e_len, char* res, uint32_t res_len);

__attribute__((eosio_wasm_import))
int32_t bls_g2_map(const char* e, uint32_t e_len, char* res, uint32_t res_len);

__attribute__((eosio_wasm_import))
int32_t bls_fp_mod(const char* s, uint32_t s_len, char* res, uint32_t res_len);

__attribute__((eosio_wasm_import))
int32_t bls_fp_mul(const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* res, uint32_t res_len);

__attribute__((eosio_wasm_import))
int32_t bls_fp_exp(const char* base, uint32_t base_len, const char* exp, uint32_t exp_len, char* res, uint32_t res_len);

#ifdef __cplusplus
}
#endif

