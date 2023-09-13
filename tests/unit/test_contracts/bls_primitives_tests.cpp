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

        [[eosio::action]]
        void popverifyraw(const std::vector<char>& pk, const std::vector<char>& sig) {
            check(pk.size() == std::tuple_size<bls_g1_affine>::value, "wrong public key size passed");
            check(sig.size() == std::tuple_size<bls_g2_affine>::value, "wrong signature size passed");
            check(bls_pop_verify(*reinterpret_cast<const bls_g1_affine*>(pk.data()), *reinterpret_cast<const bls_g2_affine*>(sig.data())), "raw pop verify failed");
        }

        [[eosio::action]]
        void popverify(const std::string& pk, const std::string& sig) {
            check(bls_pop_verify(bls_base64_to_g1_affine(pk.data(), pk.size()), bls_base64_to_sig_affine(sig.data(), sig.size())), "pop verify failed");
        }

        [[eosio::action]]
        void g1baseb4enc(const std::vector<char>& g1, const std::string& base64 ) {
            check(g1.size() == std::tuple_size<bls_g1_affine>::value, "wrong g1 size passed");

            check(bls_g1_affine_to_base64(*reinterpret_cast<const bls_g1_affine*>(g1.data())) == base64, "g1 to base64 encoding doesn't match" );
            check(bls_base64_to_g1_affine(base64.data(), base64.size()) == *reinterpret_cast<const bls_g1_affine*>(g1.data()), "base64 to g1 decoding doesn't match" );
        }

        [[eosio::action]]
        void sigbaseb4enc(const std::vector<char>& g2, const std::string& base64) {
            check(g2.size() == std::tuple_size<bls_g2_affine>::value, "wrong g2 size passed");

            check(bls_sig_to_base64_affine(*reinterpret_cast<const bls_g2_affine*>(g2.data())) == base64, "g2 to base64 encoding doesn't match" );
            check(bls_base64_to_sig_affine(base64.data(), base64.size()) == *reinterpret_cast<const bls_g2_affine*>(g2.data()), "base64 to g2 decoding doesn't match" );
        }
};
