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
        }
    }

    using bls_scalar = std::array<char, 32>;
    using bls_fp     = std::array<char, 48>;
    using bls_s      = std::array<char, 64>;
    using bls_fp2    = std::array<bls_fp, 2>;
    using bls_g1     = std::array<char, 96>;
    using bls_g2     = std::array<char, 192>;
    using bls_gt     = std::array<char, 576>; // group fp12

    inline int32_t bls_g1_add(const bls_g1& op1, const bls_g1& op2, bls_g1& res) {
        return internal_use_do_not_use::bls_g1_add(
            op1.data(), op1.size(),
            op2.data(), op2.size(),
            res.data(), res.size()
        );
    }

    inline int32_t bls_g2_add(const bls_g2& op1, const bls_g2& op2, bls_g2& res) {
        return internal_use_do_not_use::bls_g2_add(
            op1.data(), op1.size(),
            op2.data(), op2.size(),
            res.data(), res.size()
        );
    }

    inline int32_t bls_g1_weighted_sum(const bls_g1 g1_points[], const bls_scalar scalars[], uint32_t num, bls_g1& res) {
        if (num > 0) {
            return internal_use_do_not_use::bls_g1_weighted_sum(
                g1_points[0].data(), num * g1_points[0].size(),
                scalars[0].data(), num * scalars[0].size(),
                num,
                res.data(), res.size()
            );
        } else {
            return internal_use_do_not_use::bls_g1_weighted_sum(
                nullptr, 0,
                nullptr, 0,
                0,
                res.data(), res.size()
            );
        }
    }

    inline int32_t bls_g2_weighted_sum(const bls_g2 g2_points[], const bls_scalar scalars[], uint32_t num, bls_g2& res) {
        if (num > 0) {
            return internal_use_do_not_use::bls_g2_weighted_sum(
                g2_points[0].data(), num * g2_points[0].size(),
                scalars[0].data(), num * scalars[0].size(),
                num,
                res.data(), res.size()
            );
        } else {
            return internal_use_do_not_use::bls_g2_weighted_sum(
                nullptr, 0,
                nullptr, 0,
                0,
                res.data(), res.size()
            );
        }
    }

    inline int32_t bls_pairing(const bls_g1 g1_points[], const bls_g2 g2_points[], const uint32_t num, bls_gt& res) {
        if (num > 0) {
            return internal_use_do_not_use::bls_pairing(
                g1_points[0].data(), num * g1_points[0].size(),
                g2_points[0].data(), num * g2_points[0].size(),
                num,
                res.data(), res.size()
            );
        } else {
            return internal_use_do_not_use::bls_pairing(
                nullptr, 0,
                nullptr, 0,
                0,
                res.data(), res.size()
            );
        }
    }

    inline int32_t bls_g1_map(const bls_fp& e, bls_g1& res) {
        return internal_use_do_not_use::bls_g1_map(
            e.data(), e.size(),
            res.data(), res.size()
        );
    }

    inline int32_t bls_g2_map(const bls_fp2& e, bls_g2& res) {
        return internal_use_do_not_use::bls_g2_map(
            e[0].data(), 2 * e[0].size(),
            res.data(), res.size()
        );
    }

    inline int32_t bls_fp_mod(const bls_s& s, bls_fp& res) {
        return internal_use_do_not_use::bls_fp_mod(
            s.data(), s.size(),
            res.data(), res.size()
        );
    }

    inline int32_t bls_fp_mul(const bls_fp& op1, const bls_fp& op2, bls_fp& res) {
        return internal_use_do_not_use::bls_fp_mul(
            op1.data(), op1.size(),
            op2.data(), op2.size(),
            res.data(), res.size()
        );
    }

    inline int32_t bls_fp_exp(const bls_fp& base, const bls_s& exp, bls_fp& res) {
        return internal_use_do_not_use::bls_fp_exp(
            base.data(), base.size(),
            exp.data(), exp.size(),
            res.data(), res.size()
        );
    }
}

