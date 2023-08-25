#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>
#include <eosio/crypto_ext.hpp>
#include <eosio/crypto_bls_ext.hpp>

#include <endian.h>

using namespace eosio;

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

class [[eosio::contract]] bls_primitives_tests : public contract{
    public:
        using contract::contract;

        [[eosio::action]]
        void testg1add(const std::vector<char>& op1, const std::vector<char>& op2, const std::vector<char>& res) {
            check(op1.size() == std::tuple_size<bls_g1>::value, "wrong op1 size passed");
            check(op2.size() == std::tuple_size<bls_g1>::value, "wrong op2 size passed");
            bls_g1 r;
            bls_g1_add(*reinterpret_cast<const bls_g1*>(op1.data()), *reinterpret_cast<const bls_g1*>(op2.data()), r);
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_g1_add test failed");
        }

        [[eosio::action]]
        void testg2add(const std::vector<char>& op1, const std::vector<char>& op2, const std::vector<char>& res) {
            bls_g2 r;
            bls_g2_add(*reinterpret_cast<const bls_g2*>(op1.data()), *reinterpret_cast<const bls_g2*>(op2.data()), r);
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_g2_add test failed");
        }

        [[eosio::action]]
        void testg1mul(const std::vector<char>& point, const std::vector<char>& scalar, const std::vector<char>& res) {
            bls_g1 r;
            bls_g1_mul(*reinterpret_cast<const bls_g1*>(point.data()), *reinterpret_cast<const bls_scalar*>(scalar.data()), r);
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_g1_mul test failed");
        }

        [[eosio::action]]
        void testg2mul(const std::vector<char>& point, const std::vector<char>& scalar, const std::vector<char>& res) {
            bls_g2 r;
            bls_g2_mul(*reinterpret_cast<const bls_g2*>(point.data()), *reinterpret_cast<const bls_scalar*>(scalar.data()), r);
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_g2_mul test failed");
        }

        [[eosio::action]]
        void testg1exp(const std::vector<char>& points, const std::vector<char>& scalars, const std::vector<char>& res) {
            auto num = scalars.size()/sizeof(bls_scalar);
            check(points.size()/sizeof(bls_g1) == num, "number of elements in points and scalars must be equal");
            bls_g1 r;
            bls_g1_exp(reinterpret_cast<const bls_g1*>(points.data()), reinterpret_cast<const bls_scalar*>(scalars.data()), num, r);
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_g1_exp test failed");
        }

        [[eosio::action]]
        void testg2exp(const std::vector<char>& points, const std::vector<char>& scalars, const std::vector<char>& res) {
            auto num = scalars.size()/sizeof(bls_scalar);
            check(points.size()/sizeof(bls_g2) == num, "number of elements in points and scalars must be equal");
            bls_g2 r;
            bls_g2_exp(reinterpret_cast<const bls_g2*>(points.data()), reinterpret_cast<const bls_scalar*>(scalars.data()), num, r);
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_g2_exp test failed");
        }

        [[eosio::action]]
        void testpairing(const std::vector<char>& g1_points, const std::vector<char>& g2_points, const std::vector<char>& res) {
            auto num = g2_points.size()/sizeof(bls_g2);
            check(g1_points.size()/sizeof(bls_g1) == num, "number of elements in g1_points and g2_points must be equal");
            bls_gt r;
            bls_pairing(reinterpret_cast<const bls_g1*>(g1_points.data()), reinterpret_cast<const bls_g2*>(g2_points.data()), num, r);
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_pairing test failed");
        }

        [[eosio::action]]
        void testg1map(const std::vector<char>& e, const std::vector<char>& res) {
            bls_g1 r;
            bls_g1_map(*reinterpret_cast<const bls_fp*>(e.data()), r);
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_g1_map test failed");
        }

        [[eosio::action]]
        void testg2map(const std::vector<char>& e, const std::vector<char>& res) {
            bls_g2 r;
            bls_g2_map(*reinterpret_cast<const bls_fp2*>(e.data()), r);
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_g2_map test failed");
        }

        // Construct an extensible-output function based on SHA256
        void xmd_sh256(
            char *buf,
            int buf_len,
            const uint8_t *in,
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
            std::array<uint64_t, 8> out;
            for(uint64_t i = 0; i < 8; i++) {
                uint64_t temp;
                memcpy(&temp, &in[in.size() - i*8 - 8], sizeof(uint64_t));
                out[i] = htobe64(temp);
            }
            return reinterpret_cast<bls_s&&>(std::move(out));
        }

        void g2_fromMessage(const std::vector<uint8_t>& msg, const std::string& dst, bls_g2& res) {

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

        const std::string CIPHERSUITE_ID = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_NUL_";
        const std::vector<uint8_t> G1_ONE_NEG = {0x16, 0x0c, 0x53, 0xfd, 0x90, 0x87, 0xb3, 0x5c, 0xf5, 0xff, 0x76, 0x99, 0x67, 0xfc, 0x17, 0x78, 0xc1, 0xa1, 0x3b, 0x14, 0xc7, 0x95, 0x4f, 0x15, 0x47, 0xe7, 0xd0, 0xf3, 0xcd, 0x6a, 0xae, 0xf0, 0x40, 0xf4, 0xdb, 0x21, 0xcc, 0x6e, 0xce, 0xed, 0x75, 0xfb, 0x0b, 0x9e, 0x41, 0x77, 0x01, 0x12, 0x3a, 0x88, 0x18, 0xf3, 0x2a, 0x6c, 0x52, 0xff, 0x70, 0x02, 0x3b, 0x38, 0xe4, 0x9c, 0x89, 0x92, 0x55, 0xd0, 0xa9, 0x9f, 0x8d, 0x73, 0xd7, 0x89, 0x2a, 0xc1, 0x44, 0xa3, 0x5b, 0xf3, 0xca, 0x12, 0x17, 0x53, 0x4b, 0x96, 0x76, 0x1b, 0xff, 0x3c, 0x30, 0x44, 0x77, 0xe9, 0xed, 0xd2, 0x44, 0x0e, 0xfd, 0xff, 0x02, 0x00, 0x00, 0x00, 0x09, 0x76, 0x02, 0x00, 0x0c, 0xc4, 0x0b, 0x00, 0xf4, 0xeb, 0xba, 0x58, 0xc7, 0x53, 0x57, 0x98, 0x48, 0x5f, 0x45, 0x57, 0x52, 0x70, 0x53, 0x58, 0xce, 0x77, 0x6d, 0xec, 0x56, 0xa2, 0x97, 0x1a, 0x07, 0x5c, 0x93, 0xe4, 0x80, 0xfa, 0xc3, 0x5e, 0xf6, 0x15};
        const std::vector<uint8_t> GT_ONE = {0xfd, 0xff, 0x02, 0x00, 0x00, 0x00, 0x09, 0x76, 0x02, 0x00, 0x0c, 0xc4, 0x0b, 0x00, 0xf4, 0xeb, 0xba, 0x58, 0xc7, 0x53, 0x57, 0x98, 0x48, 0x5f, 0x45, 0x57, 0x52, 0x70, 0x53, 0x58, 0xce, 0x77, 0x6d, 0xec, 0x56, 0xa2, 0x97, 0x1a, 0x07, 0x5c, 0x93, 0xe4, 0x80, 0xfa, 0xc3, 0x5e, 0xf6, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        std::vector<uint8_t> msg = {51, 23, 56, 93, 212, 129, 128, 27, 251, 12, 42, 129, 210, 9, 34, 98};

        [[eosio::action]]
        void verify(const std::vector<char>& pk, const std::vector<char>& sig) {
            bls_g1 g1_points[2];
            bls_g2 g2_points[2];

            memcpy(g1_points[0].data(), G1_ONE_NEG.data(), sizeof(bls_g1));
            memcpy(g2_points[0].data(), sig.data(), sizeof(bls_g2));

            bls_g2 p_msg;
            g2_fromMessage(msg, CIPHERSUITE_ID, p_msg);
            memcpy(g1_points[1].data(), pk.data(), sizeof(bls_g1));
            memcpy(g2_points[1].data(), p_msg.data(), sizeof(bls_g2));

            bls_gt r;
            bls_pairing(g1_points, g2_points, 2, r);
            check(0 == memcmp(r.data(), GT_ONE.data(), sizeof(bls_gt)), "bls signature verify failed");
        }
};
