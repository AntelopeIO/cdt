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

    using bls_scalar    = std::array<char, 32>;
    using bls_fp        = std::array<char, 48>;
    using bls_s         = std::array<char, 64>;
    using bls_fp2       = std::array<bls_fp, 2>;
    using bls_g1        = std::array<char, 144>;
    using bls_g1_affine = std::array<char, 96>;
    using bls_g2        = std::array<char, 288>;
    using bls_gt        = std::array<char, 576>;

    int32_t bls_g1_add(const bls_g1& op1, const bls_g1& op2, bls_g1& res) {
        return internal_use_do_not_use::bls_g1_add(
            op1.data(),
            sizeof(bls_g1),
            op2.data(),
            sizeof(bls_g1),
            res.data(),
            sizeof(bls_g1)
        );
    }

    int32_t bls_g2_add(const bls_g2& op1, const bls_g2& op2, bls_g2& res) {
        return internal_use_do_not_use::bls_g2_add(
            op1.data(),
            sizeof(bls_g2),
            op2.data(),
            sizeof(bls_g2),
            res.data(),
            sizeof(bls_g2)
        );
    }

    int32_t bls_g1_mul(const bls_g1& point, const bls_scalar& scalar, bls_g1& res) {
        return internal_use_do_not_use::bls_g1_mul(
            point.data(),
            sizeof(bls_g1),
            scalar.data(),
            sizeof(bls_scalar),
            res.data(),
            sizeof(bls_g1)
        );
    }

    int32_t bls_g2_mul(const bls_g2& point, const bls_scalar& scalar, bls_g2& res) {
        return internal_use_do_not_use::bls_g2_mul(
            point.data(),
            sizeof(bls_g2),
            scalar.data(),
            sizeof(bls_scalar),
            res.data(),
            sizeof(bls_g2)
        );
    }

    int32_t bls_g1_exp(const bls_g1 points[], const bls_scalar scalars[], const uint32_t num, bls_g1& res) {
        return internal_use_do_not_use::bls_g1_exp(
            num ? points[0].data() : nullptr,
            num * sizeof(bls_g1),
            num ? scalars[0].data() : nullptr,
            num * sizeof(bls_scalar),
            num,
            res.data(),
            sizeof(bls_g1)
        );
    }

    int32_t bls_g2_exp(const bls_g2 points[], const bls_scalar scalars[], const uint32_t num, bls_g2& res) {
        return internal_use_do_not_use::bls_g2_exp(
            num ? points[0].data() : nullptr,
            num * sizeof(bls_g2),
            num ? scalars[0].data() : nullptr,
            num * sizeof(bls_scalar),
            num,
            res.data(),
            sizeof(bls_g2)
        );
    }

    int32_t bls_pairing(const bls_g1 g1_points[], const bls_g2 g2_points[], const uint32_t num, bls_gt& res) {
        return internal_use_do_not_use::bls_pairing(
            num ? g1_points[0].data() : nullptr,
            num * sizeof(bls_g1),
            num ? g2_points[0].data() : nullptr,
            num * sizeof(bls_g2),
            num,
            res.data(),
            sizeof(bls_gt)
        );
    }

    int32_t bls_g1_map(const bls_fp& e, bls_g1& res) {
        return internal_use_do_not_use::bls_g1_map(
            e.data(),
            sizeof(bls_fp),
            res.data(),
            sizeof(bls_g1)
        );
    }

    int32_t bls_g2_map(const bls_fp2& e, bls_g2& res) {
        return internal_use_do_not_use::bls_g2_map(
            e[0].data(),
            sizeof(bls_fp2),
            res.data(),
            sizeof(bls_g2)
        );
    }

    int32_t bls_fp_mod(const bls_s& s, bls_fp& res) {
        return internal_use_do_not_use::bls_fp_mod(
            s.data(),
            sizeof(bls_s),
            res.data(),
            sizeof(bls_fp)
        );
    }
}

