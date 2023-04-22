#pragma once

#include "fixed_bytes.hpp"
#include "varint.hpp"
#include "serialize.hpp"

#include <array>
#include <vector>

namespace eosio {

    using bls_scalar    = std::array<uint8_t, 32>;
    using bls_fp        = std::array<uint8_t, 48>;
    using bls_fp2       = std::array<uint8_t, 96>;
    using bls_g1        = std::array<uint8_t, 144>;
    using bls_g2        = std::array<uint8_t, 288>;
    using bls_gt        = std::array<uint8_t, 576>;

    namespace internal_use_do_not_use {
        extern "C" {
            __attribute__((eosio_wasm_import))
            void bls_g1_add(const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            void bls_g2_add(const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            void bls_g1_mul(const char* point, uint32_t point_len, const char* scalar, uint32_t scalar_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            void bls_g2_mul(const char* point, uint32_t point_len, const char* scalar, uint32_t scalar_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            void bls_g1_exp(const char* points, uint32_t points_len, const char* scalars, uint32_t scalars_len, uint32_t n, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            void bls_g2_exp(const char* points, uint32_t points_len, const char* scalars, uint32_t scalars_len, uint32_t n, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            void bls_pairing(const char* g1_points, uint32_t g1_points_len, const char* g2_points, uint32_t g2_points_len, uint32_t n, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            void bls_g1_map(const char* e, uint32_t e_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            void bls_g2_map(const char* e, uint32_t e_len, char* res, uint32_t res_len);

            __attribute__((eosio_wasm_import))
            void bls_fp_mod(const char* s, uint32_t s_len, char* res, uint32_t res_len);
        }
    }

    void bls_g1_add(const bls_g1& op1, const bls_g1& op2, bls_g1& res)
    {
        return internal_use_do_not_use::bls_g1_add(reinterpret_cast<const char*>(op1.data()), op1.size(), reinterpret_cast<const char*>(op2.data()), op2.size(), reinterpret_cast<char*>(res.data()), res.size());
    }

    void bls_g2_add(const bls_g2& op1, const bls_g2& op2, bls_g2& res)
    {
        return internal_use_do_not_use::bls_g2_add(reinterpret_cast<const char*>(op1.data()), op1.size(), reinterpret_cast<const char*>(op2.data()), op2.size(), reinterpret_cast<char*>(res.data()), res.size());
    }

    void bls_g1_mul(const bls_g1& point, const bls_scalar& scalar, bls_g1& res)
    {
        return internal_use_do_not_use::bls_g1_mul(reinterpret_cast<const char*>(point.data()), point.size(), reinterpret_cast<const char*>(scalar.data()), scalar.size(), reinterpret_cast<char*>(res.data()), res.size());
    }

    void bls_g2_mul(const bls_g2& point, const bls_scalar& scalar, bls_g2& res)
    {
        return internal_use_do_not_use::bls_g2_mul(reinterpret_cast<const char*>(point.data()), point.size(), reinterpret_cast<const char*>(scalar.data()), scalar.size(), reinterpret_cast<char*>(res.data()), res.size());
    }

    void bls_g1_exp(const std::vector<bls_g1>& points, const std::vector<bls_scalar>& scalars, bls_g1& res)
    {
        if(points.size() != scalars.size()) return;
        return internal_use_do_not_use::bls_g1_exp(reinterpret_cast<const char*>(points.data()), points.size() * sizeof(bls_g1), reinterpret_cast<const char*>(scalars.data()), scalars.size() * sizeof(bls_scalar), points.size(), reinterpret_cast<char*>(res.data()), res.size());
    }

    void bls_g2_exp(const std::vector<bls_g2>& points, const std::vector<bls_scalar>& scalars, bls_g2& res)
    {
        if(points.size() != scalars.size()) return;
        return internal_use_do_not_use::bls_g2_exp(reinterpret_cast<const char*>(points.data()), points.size() * sizeof(bls_g2), reinterpret_cast<const char*>(scalars.data()), scalars.size() * sizeof(bls_scalar), points.size(), reinterpret_cast<char*>(res.data()), res.size());
    }

    void bls_pairing(const std::vector<bls_g1>& g1_points, const std::vector<bls_g2>& g2_points, bls_gt& res)
    {
        if(g1_points.size() != g2_points.size()) return;
        return internal_use_do_not_use::bls_pairing(reinterpret_cast<const char*>(g1_points.data()), g1_points.size() * sizeof(bls_g1), reinterpret_cast<const char*>(g2_points.data()), g2_points.size() * sizeof(bls_g2), g1_points.size(), reinterpret_cast<char*>(res.data()), res.size());
    }

    void bls_g1_map(const bls_fp& e, bls_g1& res)
    {
        return internal_use_do_not_use::bls_g1_map(reinterpret_cast<const char*>(e.data()), e.size(), reinterpret_cast<char*>(res.data()), res.size());
    }

    void bls_g2_map(const bls_fp2& e, bls_g2& res)
    {
        return internal_use_do_not_use::bls_g2_map(reinterpret_cast<const char*>(e.data()), e.size(), reinterpret_cast<char*>(res.data()), res.size());
    }

    void bls_fp_mod(const std::array<uint8_t, 64>& s, bls_fp& res)
    {
        return internal_use_do_not_use::bls_fp_mod(reinterpret_cast<const char*>(s.data()), s.size(), reinterpret_cast<char*>(res.data()), res.size());
    }
}

