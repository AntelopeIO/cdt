#pragma once

#include "crypto.hpp"

#include "fixed_bytes.hpp"
#include "varint.hpp"
#include "serialize.hpp"
#include "base64.hpp"

#include <array>
#include <vector>
#include <span>

namespace bls12_381 {
class sha256 {
public:
    sha256(): m_blocklen(0), m_bitlen(0) {
        m_state[0] = 0x6a09e667;
        m_state[1] = 0xbb67ae85;
        m_state[2] = 0x3c6ef372;
        m_state[3] = 0xa54ff53a;
        m_state[4] = 0x510e527f;
        m_state[5] = 0x9b05688c;
        m_state[6] = 0x1f83d9ab;
        m_state[7] = 0x5be0cd19;
    }
    void update(const uint8_t * data, size_t length) {
        for(size_t i = 0 ; i < length ; i++) {
            m_data[m_blocklen++] = data[i];
            if (m_blocklen == 64) {
                transform();

                // End of the block
                m_bitlen += 512;
                m_blocklen = 0;
            }
        }
    }
    inline void update(const char* data, size_t length) {
        update(reinterpret_cast<const uint8_t*>(data), length);
    }
    inline void update(const std::string &data) {
        update(reinterpret_cast<const char*>(data.data()), data.size());
    }
    std::array<uint8_t, 32> digest() {
        std::array<uint8_t, 32> hash;

        pad();
        revert(hash);

        return hash;
    }
    void digest(uint8_t* dst) {
        std::array<uint8_t, 32>* phash = reinterpret_cast<std::array<uint8_t, 32>*>(dst);

        pad();
        revert(*phash);
    }

    //static string toString(const array<uint8_t, 32>& digest);

private:
    uint8_t  m_data[64];
    uint32_t m_blocklen;
    uint64_t m_bitlen;
    uint32_t m_state[8]; //A, B, C, D, E, F, G, H

    static constexpr std::array<uint32_t, 64> K = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
        0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
        0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
        0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
        0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
        0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
        0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
        0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
        0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };

    static uint32_t rotr(uint32_t x, uint32_t n) {
        return (x >> n) | (x << (32 - n));
    }
    static uint32_t choose(uint32_t e, uint32_t f, uint32_t g) {
        return (e & f) ^ (~e & g);
    }
    static uint32_t majority(uint32_t a, uint32_t b, uint32_t c) {
        return (a & (b | c)) | (b & c);
    }
    static uint32_t sig0(uint32_t x) {
        return sha256::rotr(x, 7) ^ sha256::rotr(x, 18) ^ (x >> 3);
    }
    static uint32_t sig1(uint32_t x) {
        return sha256::rotr(x, 17) ^ sha256::rotr(x, 19) ^ (x >> 10);
    }
    void transform() {
        uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
        uint32_t state[8];

        for(uint8_t i = 0, j = 0; i < 16; i++, j += 4) {
            // Split data in 32 bit blocks for the 16 first words
            m[i] = (m_data[j] << 24) | (m_data[j + 1] << 16) | (m_data[j + 2] << 8) | (m_data[j + 3]);
        }

        for(uint8_t k = 16 ; k < 64; k++) {
            // Remaining 48 blocks
            m[k] = sha256::sig1(m[k - 2]) + m[k - 7] + sha256::sig0(m[k - 15]) + m[k - 16];
        }

        for(uint8_t i = 0 ; i < 8 ; i++) {
            state[i] = m_state[i];
        }

        for(uint8_t i = 0; i < 64; i++) {
            maj   = sha256::majority(state[0], state[1], state[2]);
            xorA  = sha256::rotr(state[0], 2) ^ sha256::rotr(state[0], 13) ^ sha256::rotr(state[0], 22);

            ch = choose(state[4], state[5], state[6]);

            xorE  = sha256::rotr(state[4], 6) ^ sha256::rotr(state[4], 11) ^ sha256::rotr(state[4], 25);

            sum  = m[i] + K[i] + state[7] + ch + xorE;
            newA = xorA + maj + sum;
            newE = state[3] + sum;

            state[7] = state[6];
            state[6] = state[5];
            state[5] = state[4];
            state[4] = newE;
            state[3] = state[2];
            state[2] = state[1];
            state[1] = state[0];
            state[0] = newA;
        }

        for(uint8_t i = 0 ; i < 8 ; i++) {
            m_state[i] += state[i];
        }
    }
    void pad() {
        uint64_t i = m_blocklen;
        uint8_t end = m_blocklen < 56 ? 56 : 64;

        m_data[i++] = 0x80; // Append a bit 1
        while(i < end) {
            m_data[i++] = 0x00; // Pad with zeros
        }

        if(m_blocklen >= 56) {
            transform();
            memset(m_data, 0, 56);
        }

        // Append to the padding the total message's length in bits and transform.
        m_bitlen += m_blocklen * 8;
        m_data[63] = m_bitlen;
        m_data[62] = m_bitlen >> 8;
        m_data[61] = m_bitlen >> 16;
        m_data[60] = m_bitlen >> 24;
        m_data[59] = m_bitlen >> 32;
        m_data[58] = m_bitlen >> 40;
        m_data[57] = m_bitlen >> 48;
        m_data[56] = m_bitlen >> 56;
        transform();
    }
    void revert(std::array<uint8_t, 32>& hash) {
        // SHA uses big endian byte ordering
        // Revert all bytes
        for(uint8_t i = 0 ; i < 4 ; i++) {
            for(uint8_t j = 0 ; j < 8 ; j++) {
                hash[i + (j * 4)] = (m_state[j] >> (24 - i * 8)) & 0x000000ff;
            }
        }
    }
};
} // namespace bls12_381

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
    using bls_g1     = std::array<char, 96>;  // affine little-endian
    using bls_g2     = std::array<char, 192>; // affine little-endian
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
        if (num == 0)
           return return_code::failure;
        return internal_use_do_not_use::bls_g1_weighted_sum(
            g1_points[0].data(), num * g1_points[0].size(),
            scalars[0].data(), num * scalars[0].size(),
            num,
            res.data(), res.size()
        );
    }

    inline int32_t bls_g2_weighted_sum(const bls_g2 g2_points[], const bls_scalar scalars[], uint32_t num, bls_g2& res) {
        if (num == 0)
           return return_code::failure;
        return internal_use_do_not_use::bls_g2_weighted_sum(
            g2_points[0].data(), num * g2_points[0].size(),
            scalars[0].data(), num * scalars[0].size(),
            num,
            res.data(), res.size()
        );
    }

    inline int32_t bls_pairing(const bls_g1 g1_points[], const bls_g2 g2_points[], const uint32_t num, bls_gt& res) {
        if (num == 0)
           return return_code::failure;
        return internal_use_do_not_use::bls_pairing(
            g1_points[0].data(), num * g1_points[0].size(),
            g2_points[0].data(), num * g2_points[0].size(),
            num,
            res.data(), res.size()
        );
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

namespace detail {
    const inline std::string bls_public_key_prefix = "PUB_BLS_";
    constexpr inline uint32_t bls_checksum_size = sizeof(uint32_t);
    const inline std::string bls_signature_prefix = "SIG_BLS_";

    template<typename T, const std::string& Prefix>
    std::string bls_type_to_base64(const T& g1) {
        std::array<char, sizeof(T) + bls_checksum_size> g1_with_checksum;
        auto it = std::copy(g1.begin(), g1.end(), g1_with_checksum.begin());
        
        auto csum = ripemd160(g1.data(), g1.size()).extract_as_byte_array();
        std::copy(reinterpret_cast<const char*>(csum.data()), 
                  reinterpret_cast<const char*>(csum.data())+bls_checksum_size, 
                  it);
        
        return Prefix + eosio::base64_encode({g1_with_checksum.data(), g1_with_checksum.size()});
    }

    template<typename T, const std::string& Prefix>
    T bls_base64_to_type(const char* data, size_t size) {
        eosio::check(size > Prefix.size(), "encoded base64 key is too short");
        eosio::check(0 == memcmp(data, Prefix.data(), Prefix.size()), "base64 encoded type must begin from corresponding prefix");

        std::string decoded = eosio::base64_decode({data+Prefix.size(), size - Prefix.size()});
        T ret;
        eosio::check(decoded.size() == ret.size() + bls_checksum_size, "decoded size " + std::to_string(decoded.size()) + 
                                                                                   " doesn't match structure size " + std::to_string(ret.size()) + 
                                                                                   " + checksum " + std::to_string(bls_checksum_size));
        
        auto it = decoded.end();
        std::advance(it, -bls_checksum_size);
        std::copy(decoded.begin(), it, ret.begin());
        
        auto csum = ripemd160(ret.data(), ret.size()).extract_as_byte_array();
        eosio::check(0 == memcmp(&*it, csum.data(), bls_checksum_size), "checksum of structure doesn't match");

        return ret;
    }

    const inline std::string CIPHERSUITE_ID     = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_NUL_"; // basic DST
    const inline std::string POP_CIPHERSUITE_ID = "BLS_POP_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"; // proof of possession DST

    // g1::one().negate().toAffineBytesLE(), 96 bytes
    const inline std::vector<uint8_t> G1_ONE_NEG = {0xbb, 0xc6, 0x22, 0xdb, 0xa, 0xf0, 0x3a, 0xfb, 0xef, 0x1a, 0x7a, 0xf9,
                                                    0x3f, 0xe8, 0x55, 0x6c, 0x58, 0xac, 0x1b, 0x17, 0x3f, 0x3a, 0x4e, 0xa1,
                                                    0x5, 0xb9, 0x74, 0x97, 0x4f, 0x8c, 0x68, 0xc3, 0xf, 0xac, 0xa9, 0x4f,
                                                    0x8c, 0x63, 0x95, 0x26, 0x94, 0xd7, 0x97, 0x31, 0xa7, 0xd3, 0xf1, 0x17,
                                                    0xca, 0xc2, 0x39, 0xb9, 0xd6, 0xdc, 0x54, 0xad, 0x1b, 0x75, 0xcb, 0xe,
                                                    0xba, 0x38, 0x6f, 0x4e, 0x36, 0x42, 0xac, 0xca, 0xd5, 0xb9, 0x55, 0x66,
                                                    0xc9, 0x7, 0xb5, 0x1d, 0xef, 0x6a, 0x81, 0x67, 0xf2, 0x21, 0x2e, 0xcf,
                                                    0xc8, 0x76, 0x7d, 0xaa, 0xa8, 0x45, 0xd5, 0x55, 0x68, 0x1d, 0x4d, 0x11};

    // fp12::one().toBytesLE(), 576 bytes
    const inline std::vector<uint8_t> GT_ONE = []{ std::vector<uint8_t> r(576, 0); r[0] = 1; return r; }();

    // Construct an extensible-output function based on SHA256
    void xmd_sh256(
        char *buf,
        int buf_len,
        const char *in,
        int in_len,
        const char *dst,
        int dst_len
    ) {
        const unsigned int SHA256HashSize = 32;
        const unsigned int SHA256_Message_Block_Size = 64;
        const unsigned ell = (buf_len + SHA256HashSize - 1) / SHA256HashSize;
        if (buf_len < 0 || ell > 255 || dst_len > 255) {
            return;
        }
        const uint8_t Z_pad[SHA256_Message_Block_Size] = { 0, };
        const uint8_t l_i_b_0_str[] = {
            static_cast<uint8_t>(buf_len >> 8),
            static_cast<uint8_t>(buf_len & 0xff),
            0,
            static_cast<uint8_t>(dst_len)
        };
        const uint8_t *dstlen_str = l_i_b_0_str + 3;
        uint8_t b_0[SHA256HashSize];
        bls12_381::sha256 sha;
        sha.update(Z_pad, SHA256_Message_Block_Size);
        sha.update(in, in_len);
        sha.update(l_i_b_0_str, 3);
        sha.update(dst, dst_len);
        sha.update(dstlen_str, 1);
        sha.digest(b_0);
        uint8_t b_i[SHA256HashSize + 1] = { 0, };
        for (unsigned i = 1; i <= ell; ++i) {
            for (unsigned j = 0; j < SHA256HashSize; ++j) {
                b_i[j] = b_0[j] ^ b_i[j];
            }
            b_i[SHA256HashSize] = i;
            bls12_381::sha256 s;
            s.update(b_i, SHA256HashSize + 1);
            s.update(dst, dst_len);
            s.update(dstlen_str, 1);
            s.digest(b_i);
            const int rem_after = buf_len - i * SHA256HashSize;
            const int copy_len = SHA256HashSize + (rem_after < 0 ? rem_after : 0);
            memcpy(buf + (i - 1) * SHA256HashSize, b_i, copy_len);
        }
    }

    bls_s scalar_fromBE(const bls_s& in) {
        bls_s out;
        constexpr size_t last_index = sizeof(out) - 1;
        for(size_t i = 0; i <= last_index; ++i)
        {
            out[i] = in[last_index - i];
        }
        return out;
    }

    void g2_fromMessage(std::span<const char> msg, const std::string& dst, bls_g2& res) {

        std::array<bls_s, 4> buf;
        xmd_sh256(buf.data()->data(), sizeof(buf), msg.data(), msg.size(), dst.data(), dst.length());

        bls_s k;
        bls_fp2 t;
        bls_g2 p, q;

        k = scalar_fromBE(buf[0]);
        bls_fp_mod(k, t[0]);
        k = scalar_fromBE(buf[1]);
        bls_fp_mod(k, t[1]);

        bls_g2_map(t, p);

        k = scalar_fromBE(buf[2]);
        bls_fp_mod(k, t[0]);
        k = scalar_fromBE(buf[3]);
        bls_fp_mod(k, t[1]);

        bls_g2_map(t, q);
        bls_g2_add(p, q, res);
    }
} // namespace detail

    inline std::string encode_g1_to_bls_public_key(const bls_g1& g1) {
        return detail::bls_type_to_base64<bls_g1, detail::bls_public_key_prefix>(g1);
    }
    inline bls_g1 decode_bls_public_key_to_g1(std::string_view public_key) {
        return detail::bls_base64_to_type<bls_g1, detail::bls_public_key_prefix>(public_key.data(), public_key.size());
    }
    inline std::string encode_g2_to_bls_signature(const bls_g2& g2) {
        return detail::bls_type_to_base64<bls_g2, detail::bls_signature_prefix>(g2);
    }
    inline bls_g2 decode_bls_signature_to_g2(std::string_view public_key) {
        return detail::bls_base64_to_type<bls_g2, detail::bls_signature_prefix>(public_key.data(), public_key.size());
    }

    // pubkey and signature are assumed to be in RAW affine little-endian bytes
    bool bls_pop_verify(const bls_g1& pubkey, const bls_g2& signature_proof) {
        using namespace detail;

        bls_g1 g1_points[2] = {0};
        bls_g2 g2_points[2] = {0};

        std::memcpy(g1_points[0].data(), G1_ONE_NEG.data(), G1_ONE_NEG.size());
        std::memcpy(g2_points[0].data(), signature_proof.data(), signature_proof.size());

        std::memcpy(g1_points[1].data(), pubkey.data(), pubkey.size());
        g2_fromMessage(pubkey, POP_CIPHERSUITE_ID, g2_points[1]);

        bls_gt r;
        bls_pairing(g1_points, g2_points, 2, r);

        return 0 == std::memcmp(r.data(), GT_ONE.data(), GT_ONE.size());
    }

    // pubkey and signature are assumed to be in RAW affine little-endian bytes
    bool bls_signature_verify(const bls_g1& pubkey, const bls_g2& signature_proof, const std::string& msg) {
        bls_g1 g1_points[2];
        bls_g2 g2_points[2];

        std::memcpy(g1_points[0].data(), detail::G1_ONE_NEG.data(), detail::G1_ONE_NEG.size());
        std::memcpy(g2_points[0].data(), signature_proof.data(), signature_proof.size());

        std::memcpy(g1_points[1].data(), pubkey.data(), pubkey.size());
        detail::g2_fromMessage(msg, detail::CIPHERSUITE_ID, g2_points[1]);

        bls_gt r;
        bls_pairing(g1_points, g2_points, 2, r);

        return  0 == std::memcmp(r.data(), detail::GT_ONE.data(), detail::GT_ONE.size());
    }
}

