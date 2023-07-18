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

    using bls_scalar = uint8_t[32];
    using bls_fp = uint8_t[48];
    using bls_fp2 = bls_fp[2];
    using bls_g1 = uint8_t[144];
    using bls_g2 = uint8_t[288];
    using bls_gt = uint8_t[576];

    int32_t bls_g1_add(const bls_g1& op1, const bls_g1& op2, bls_g1& res)
    {
        return internal_use_do_not_use::bls_g1_add(
            reinterpret_cast<const char*>(op1),
            sizeof(bls_g1),
            reinterpret_cast<const char*>(op2),
            sizeof(bls_g1),
            reinterpret_cast<char*>(res),
            sizeof(bls_g1)
        );
    }

    int32_t bls_g2_add(const bls_g2& op1, const bls_g2& op2, bls_g2& res)
    {
        return internal_use_do_not_use::bls_g2_add(
            reinterpret_cast<const char*>(op1),
            sizeof(bls_g2),
            reinterpret_cast<const char*>(op2),
            sizeof(bls_g2),
            reinterpret_cast<char*>(res),
            sizeof(bls_g2)
        );
    }

    int32_t bls_g1_mul(const bls_g1& point, const bls_scalar& scalar, bls_g1& res)
    {
        return internal_use_do_not_use::bls_g1_mul(
            reinterpret_cast<const char*>(point),
            sizeof(bls_g1),
            reinterpret_cast<const char*>(scalar),
            sizeof(bls_scalar),
            reinterpret_cast<char*>(res),
            sizeof(bls_g1)
        );
    }

    int32_t bls_g2_mul(const bls_g2& point, const bls_scalar& scalar, bls_g2& res)
    {
        return internal_use_do_not_use::bls_g2_mul(
            reinterpret_cast<const char*>(point),
            sizeof(bls_g2),
            reinterpret_cast<const char*>(scalar),
            sizeof(bls_scalar),
            reinterpret_cast<char*>(res),
            sizeof(bls_g2)
        );
    }

    int32_t bls_g1_exp(const bls_g1* points, const bls_scalar* scalars, const uint32_t num, bls_g1& res)
    {
        return internal_use_do_not_use::bls_g1_exp(
            reinterpret_cast<const char*>(points),
            num * sizeof(bls_g1),
            reinterpret_cast<const char*>(scalars),
            num * sizeof(bls_scalar),
            num,
            reinterpret_cast<char*>(res),
            sizeof(bls_g1)
        );
    }

    int32_t bls_g2_exp(const bls_g2* points, const bls_scalar* scalars, const uint32_t num, bls_g2& res)
    {
        return internal_use_do_not_use::bls_g2_exp(
            reinterpret_cast<const char*>(points),
            num * sizeof(bls_g2),
            reinterpret_cast<const char*>(scalars),
            num * sizeof(bls_scalar),
            num,
            reinterpret_cast<char*>(res),
            sizeof(bls_g2)
        );
    }

    int32_t bls_pairing(const bls_g1* g1_points, const bls_g2* g2_points, const uint32_t num, bls_gt& res)
    {
        return internal_use_do_not_use::bls_pairing(
            reinterpret_cast<const char*>(g1_points),
            num * sizeof(bls_g1),
            reinterpret_cast<const char*>(g2_points),
            num * sizeof(bls_g2),
            num,
            reinterpret_cast<char*>(res),
            sizeof(bls_gt)
        );
    }

    int32_t bls_g1_map(const bls_fp& e, bls_g1& res)
    {
        return internal_use_do_not_use::bls_g1_map(
            reinterpret_cast<const char*>(e),
            sizeof(bls_fp),
            reinterpret_cast<char*>(res),
            sizeof(bls_g1)
        );
    }

    int32_t bls_g2_map(const bls_fp2& e, bls_g2& res)
    {
        return internal_use_do_not_use::bls_g2_map(
            reinterpret_cast<const char*>(e),
            sizeof(bls_fp2),
            reinterpret_cast<char*>(res),
            sizeof(bls_g2)
        );
    }

    int32_t bls_fp_mod(const uint8_t* s, bls_fp& res)
    {
        return internal_use_do_not_use::bls_fp_mod(
            reinterpret_cast<const char*>(s),
            64,
            reinterpret_cast<char*>(res),
            sizeof(bls_fp)
        );
    }
}

