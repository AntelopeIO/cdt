#pragma once

#include "fixed_bytes.hpp"
#include "varint.hpp"
#include "serialize.hpp"

#include <array>
#include <vector>

namespace eosio {

    namespace internal_use_do_not_use {
        extern "C" {
            __attribute__((eosio_wasm_import))
            int32_t bls_g1_add(const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            int32_t bls_g2_add(const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            int32_t bls_g1_mul(const char* point, uint32_t point_len, const char* scalar, uint32_t scalar_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            int32_t bls_g2_mul(const char* point, uint32_t point_len, const char* scalar, uint32_t scalar_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            int32_t bls_g1_exp(const char* points, uint32_t points_len, const char* scalars, uint32_t scalars_len, uint32_t n, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            int32_t bls_g2_exp(const char* points, uint32_t points_len, const char* scalars, uint32_t scalars_len, uint32_t n, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            int32_t bls_pairing(const char* g1_points, uint32_t g1_points_len, const char* g2_points, uint32_t g2_points_len, uint32_t n, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            int32_t bls_g1_map(const char* e, uint32_t e_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            int32_t bls_g2_map(const char* e, uint32_t e_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            int32_t bls_fp_mod(const char* s, uint32_t s_len, char* res, uint32_t res_len);
        }
    }

    int32_t bls_g1_add(const uint8_t* op1, const uint8_t* op2, uint8_t* res)
    {
        return internal_use_do_not_use::bls_g1_add(reinterpret_cast<const char*>(op1), 144, reinterpret_cast<const char*>(op2), 144, reinterpret_cast<char*>(res), 144);
    }

    int32_t bls_g2_add(const uint8_t* op1, const uint8_t* op2, uint8_t* res)
    {
        return internal_use_do_not_use::bls_g2_add(reinterpret_cast<const char*>(op1), 288, reinterpret_cast<const char*>(op2), 288, reinterpret_cast<char*>(res), 288);
    }

    int32_t bls_g1_mul(const uint8_t* point, const uint8_t* scalar, uint8_t* res)
    {
        return internal_use_do_not_use::bls_g1_mul(reinterpret_cast<const char*>(point), 144, reinterpret_cast<const char*>(scalar), 32, reinterpret_cast<char*>(res), 144);
    }

    int32_t bls_g2_mul(const uint8_t* point, const uint8_t* scalar, uint8_t* res)
    {
        return internal_use_do_not_use::bls_g2_mul(reinterpret_cast<const char*>(point), 288, reinterpret_cast<const char*>(scalar), 32, reinterpret_cast<char*>(res), 288);
    }

    int32_t bls_g1_exp(const uint8_t* points, const uint8_t* scalars, const uint32_t num, uint8_t* res)
    {
        return internal_use_do_not_use::bls_g1_exp(reinterpret_cast<const char*>(points), num * 144, reinterpret_cast<const char*>(scalars), num * 32, num, reinterpret_cast<char*>(res), 144);
    }

    int32_t bls_g2_exp(const uint8_t* points, const uint8_t* scalars, const uint32_t num, uint8_t* res)
    {
        return internal_use_do_not_use::bls_g2_exp(reinterpret_cast<const char*>(points), num * 288, reinterpret_cast<const char*>(scalars), num * 32, num, reinterpret_cast<char*>(res), 288);
    }

    int32_t bls_pairing(const uint8_t* g1_points, const uint8_t* g2_points, const uint32_t num, uint8_t* res)
    {
        return internal_use_do_not_use::bls_pairing(reinterpret_cast<const char*>(g1_points), num * 144, reinterpret_cast<const char*>(g2_points), num * 288, num, reinterpret_cast<char*>(res), 576);
    }

    int32_t bls_g1_map(const uint8_t* e, uint8_t* res)
    {
        return internal_use_do_not_use::bls_g1_map(reinterpret_cast<const char*>(e), 48, reinterpret_cast<char*>(res), 144);
    }

    int32_t bls_g2_map(const uint8_t* e, uint8_t* res)
    {
        return internal_use_do_not_use::bls_g2_map(reinterpret_cast<const char*>(e), 96, reinterpret_cast<char*>(res), 288);
    }

    int32_t bls_fp_mod(const uint8_t* s, uint8_t* res)
    {
        return internal_use_do_not_use::bls_fp_mod(reinterpret_cast<const char*>(s), 64, reinterpret_cast<char*>(res), 48);
    }
}

