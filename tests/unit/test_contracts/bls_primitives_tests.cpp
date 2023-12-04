#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>
#include <eosio/crypto_ext.hpp>
#include <eosio/crypto_bls_ext.hpp>

using namespace eosio;

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
        void testg1wsum(const std::vector<char>& points, const std::vector<char>& scalars, const uint32_t num, const std::vector<char>& res)
        {
            check(points.size() == std::tuple_size<bls_g1>::value * num, "wrong points size passed");
            check(scalars.size() == std::tuple_size<bls_scalar>::value * num, "wrong scalars size passed");
            check(res.size() == std::tuple_size<bls_g1>::value, "wrong res size passed");
            bls_g1 r;
            int32_t error = bls_g1_weighted_sum(
                reinterpret_cast<const bls_g1*>(points.data()),
                reinterpret_cast<const bls_scalar*>(scalars.data()),
                num,
                r
            );
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_g1_weighted_sum: Result does not match");
        }

        [[eosio::action]]
        void testg2wsum(const std::vector<char>& points, const std::vector<char>& scalars, const uint32_t num, const std::vector<char>& res)
        {
            check(points.size() == std::tuple_size<bls_g2>::value * num, "wrong points size passed");
            check(scalars.size() == std::tuple_size<bls_scalar>::value * num, "wrong scalars size passed");
            check(res.size() == std::tuple_size<bls_g2>::value, "wrong res size passed");
            bls_g2 r;
            int32_t error = bls_g2_weighted_sum(
                reinterpret_cast<const bls_g2*>(points.data()),
                reinterpret_cast<const bls_scalar*>(scalars.data()),
                num,
                r
            );
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_g2_weighted_sum: Result does not match");
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

        [[eosio::action]]
        void testfpmul(const std::vector<char>& op1, const std::vector<char>& op2, const std::vector<char>& res)
        {
            bls_fp r;
            int32_t error = bls_fp_mul(
                *reinterpret_cast<const bls_fp*>(op1.data()),
                *reinterpret_cast<const bls_fp*>(op2.data()),
                r
            );
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_fp_mul: Result does not match");
        }

        [[eosio::action]]
        void testfpexp(const std::vector<char>& base, const std::vector<char>& exp, const std::vector<char>& res)
        {
            bls_fp r;
            int32_t error = bls_fp_exp(
                *reinterpret_cast<const bls_fp*>(base.data()),
                *reinterpret_cast<const bls_s*>(exp.data()),
                r
            );
            check(std::equal(res.begin(), res.end(), r.begin()), "bls_fp_exp: Result does not match");
        }

        [[eosio::action]]
        void popverifyraw(const std::vector<char>& pk, const std::vector<char>& sig) {
            check(pk.size() == std::tuple_size<bls_g1_affine>::value, "wrong public key size passed");
            check(sig.size() == std::tuple_size<bls_g2_affine>::value, "wrong signature size passed");
            check(bls_pop_verify(*reinterpret_cast<const bls_g1_affine*>(pk.data()), *reinterpret_cast<const bls_g2_affine*>(sig.data())), "raw pop verify failed");
        }

        [[eosio::action]]
        void popverify(const std::string& pk, const std::string& sig) {
            check(bls_pop_verify(decode_bls_public_key_to_g1(pk), decode_bls_signature_to_g2(sig)), "pop verify failed");
        }

        [[eosio::action]]
        void g1baseb4enc(const std::vector<char>& g1, const std::string& base64 ) {
            check(g1.size() == std::tuple_size<bls_g1_affine>::value, "wrong g1 size passed");

            check(encode_g1_to_bls_public_key(*reinterpret_cast<const bls_g1_affine*>(g1.data())) == base64, "g1 to base64 encoding doesn't match" );
            check(decode_bls_public_key_to_g1(base64) == *reinterpret_cast<const bls_g1_affine*>(g1.data()), "base64 to g1 decoding doesn't match" );
        }

        [[eosio::action]]
        void sigbaseb4enc(const std::vector<char>& g2, const std::string& base64) {
            check(g2.size() == std::tuple_size<bls_g2_affine>::value, "wrong g2 size passed");
            check(encode_g2_to_bls_signature(*reinterpret_cast<const bls_g2_affine*>(g2.data())) == base64, "g2 to base64 encoding doesn't match" );
            check(decode_bls_signature_to_g2(base64) == *reinterpret_cast<const bls_g2_affine*>(g2.data()), "base64 to g2 decoding doesn't match" );
        }

        const std::string CIPHERSUITE_ID = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_NUL_";
        // g1::one().negate().toAffineBytesLE()
        const std::vector<uint8_t> G1_ONE_NEG = {0xbb, 0xc6, 0x22, 0xdb, 0xa, 0xf0, 0x3a, 0xfb, 0xef, 0x1a, 0x7a, 0xf9, 0x3f, 0xe8, 0x55, 0x6c, 0x58, 0xac, 0x1b, 0x17, 0x3f, 0x3a, 0x4e, 0xa1, 0x5, 0xb9, 0x74, 0x97, 0x4f, 0x8c, 0x68, 0xc3, 0xf, 0xac, 0xa9, 0x4f, 0x8c, 0x63, 0x95, 0x26, 0x94, 0xd7, 0x97, 0x31, 0xa7, 0xd3, 0xf1, 0x17, 0xca, 0xc2, 0x39, 0xb9, 0xd6, 0xdc, 0x54, 0xad, 0x1b, 0x75, 0xcb, 0xe, 0xba, 0x38, 0x6f, 0x4e, 0x36, 0x42, 0xac, 0xca, 0xd5, 0xb9, 0x55, 0x66, 0xc9, 0x7, 0xb5, 0x1d, 0xef, 0x6a, 0x81, 0x67, 0xf2, 0x21, 0x2e, 0xcf, 0xc8, 0x76, 0x7d, 0xaa, 0xa8, 0x45, 0xd5, 0x55, 0x68, 0x1d, 0x4d, 0x11};
        // fp12::one().toBytesLE();
        const std::vector<uint8_t> GT_ONE = []{ std::vector<uint8_t> r(576, 0); r[0] = 1; return r; }();
        // caller of verify() must use this msg
        std::vector<uint8_t> msg = {51, 23, 56, 93, 212, 129, 128, 27, 251, 12, 42, 129, 210, 9, 34, 98};

        [[eosio::action]]
        void verify(const std::vector<char>& pk, const std::vector<char>& sig) {
            check(pk.size() == std::tuple_size<bls_g1>::value, "wrong pk size passed");
            check(sig.size() == std::tuple_size<bls_g2>::value, "wrong sig size passed");
            bls_g1 g1_points[2];
            bls_g2 g2_points[2];

            memcpy(g1_points[0].data(), G1_ONE_NEG.data(), sizeof(bls_g1));
            memcpy(g2_points[0].data(), sig.data(), sizeof(bls_g2));
        }

};
