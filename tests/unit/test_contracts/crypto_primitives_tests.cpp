#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>
#include <eosio/crypto_ext.hpp>

using namespace eosio;

class [[eosio::contract]] crypto_primitives_tests : public contract{
   public:
      using contract::contract;

      [[eosio::action]]
      void sha3test(std::string val, eosio::checksum256 sha3_dg) {
         auto hash = eosio::sha3(val.c_str(), val.size());

         eosio::check(hash == sha3_dg, "SHA3 doesn't match expected");
         eosio::assert_sha3(val.c_str(), val.size(), sha3_dg);
      }

      [[eosio::action]]
      void keccaktest(std::string val, eosio::checksum256 sha3_dg) {
         auto hash = eosio::keccak(val.c_str(), val.size());

         eosio::check(hash == sha3_dg, "Keccak doesn't match expected");
         eosio::assert_keccak(val.c_str(), val.size(), sha3_dg);
      }

      void addtest_helper(eosio::g1_view& p1, eosio::g1_view& p2, int32_t expected_rc, std::vector<char>& expected_x, std::vector<char>& expected_y) {
         std::vector<char> result_x(eosio::g1_coordinate_size);
         std::vector<char> result_y(eosio::g1_coordinate_size);
         eosio::g1_view result {result_x, result_y};

         auto rc = eosio::alt_bn128_add(p1, p2, result);
         eosio::check(rc == expected_rc, "alt_bn128_add: return code not match");

         std::vector<char> rslt_x(result.x, result.x + result.size);
         std::vector<char> rslt_y(result.y, result.y + result.size);
         eosio::check(rslt_x == expected_x, "alt_bn128_add: result x does not match");
         eosio::check(rslt_y == expected_y, "alt_bn128_add: result y does not match");
      }

      [[eosio::action]]
      void addtest(std::vector<char>& x1, std::vector<char>& y1, std::vector<char>& x2, std::vector<char>& y2, int32_t expected_rc, std::vector<char>& expected_x, std::vector<char>& expected_y) {
         eosio::g1_view point1 {x1, y1};
         eosio::g1_view point2 {x2, y2};

         addtest_helper( point1, point2, expected_rc, expected_x, expected_y );
      }

      [[eosio::action]]
      void addtest1(std::vector<char>& p1, std::vector<char>& p2, int32_t expected_rc, std::vector<char>& expected_x, std::vector<char>& expected_y) {
         eosio::g1_view point1 {p1};
         eosio::g1_view point2 {p2};

         addtest_helper( point1, point2, expected_rc, expected_x, expected_y );
      }

      [[eosio::action]]
      void multest(std::vector<char>& g1_x, std::vector<char>& g1_y, std::vector<char>& scalar, int32_t expected_rc, std::vector<char>& expected_x, std::vector<char>& expected_y) {
         eosio::g1_view g1 {g1_x, g1_y};
         eosio::bigint s {scalar};
         std::vector<char> result_x(eosio::g1_coordinate_size);
         std::vector<char> result_y(eosio::g1_coordinate_size);
         eosio::g1_view result {result_x, result_y};

         auto rc = eosio::alt_bn128_mul(g1, s, result);
         eosio::check(rc == expected_rc, "alt_bn128_mul: return code not match");

         std::vector<char> rslt_x(result.x, result.x + result.size);
         std::vector<char> rslt_y(result.y, result.y + result.size);
         eosio::check(rslt_x == expected_x, "alt_bn128_mul: Result x does not match");
         eosio::check(rslt_y == expected_y, "alt_bn128_mul: Result y does not match");
      }

      void pairtest_helper(eosio::g1_view& g1_a, eosio::g2_view& g2_a, eosio::g1_view& g1_b, eosio::g2_view& g2_b, int32_t expected) {
         std::vector<std::pair<g1_view, g2_view>> pairs { {g1_a, g2_a}, {g1_b, g2_b} };

         auto rc = eosio::alt_bn128_pair(pairs);
         eosio::check(rc == expected, "alt_bn128_pair::return code not match");
      }

      [[eosio::action]]
      void pairtest(std::vector<char>& g1_a_x, std::vector<char>& g1_a_y, std::vector<char>& g2_a_x, std::vector<char>& g2_a_y, std::vector<char>& g1_b_x, std::vector<char>& g1_b_y, std::vector<char>& g2_b_x, std::vector<char>& g2_b_y, int32_t expected) {
         eosio::g1_view g1_a {g1_a_x, g1_a_y};
         eosio::g2_view g2_a {g2_a_x, g2_a_y};
         eosio::g1_view g1_b {g1_b_x, g1_b_y};
         eosio::g2_view g2_b {g2_b_x, g2_b_y};

         pairtest_helper( g1_a, g2_a, g1_b, g2_b, expected );
      }

      [[eosio::action]]
      void pairtest1(std::vector<char>& g1a, std::vector<char>& g2a, std::vector<char>& g1b, std::vector<char>& g2b, int32_t expected) {
         eosio::g1_view g1_a { g1a };
         eosio::g2_view g2_a { g2a };
         eosio::g1_view g1_b { g1b };
         eosio::g2_view g2_b { g2b };

         pairtest_helper( g1_a, g2_a, g1_b, g2_b, expected );
      }

      [[eosio::action]]
      void modexptest(std::vector<char>& base, std::vector<char>& exp, std::vector<char>& modulo, int32_t expected_rc, const std::vector<char>& expected_result) {
         eosio::bigint base_val {base};
         eosio::bigint exp_val {exp};
         eosio::bigint modulo_val {modulo};
         std::vector<char> reslut_buf(modulo.size(), '\0');
         eosio::bigint result {reslut_buf};

         auto rc = eosio::mod_exp(base_val, exp_val, modulo_val, result);
         eosio::check(rc == expected_rc, "return code does not match");

         std::vector<char> actual_result(result.data, result.data + result.size);
         eosio::check(actual_result == expected_result, "Result does not match");
      }

      [[eosio::action]]
      void blake2ftest(uint32_t rounds, const std::vector<char>& state, const std::vector<char>& msg, const std::vector<char>& t0_offset, const std::vector<char>& t1_offset, bool final, int32_t expected_rc, const std::vector<char>& expected_result) {
         std::vector<char> result(eosio::blake2f_result_size);

         auto rc = eosio::blake2_f(rounds, state, msg, t0_offset, t1_offset, final, result);
         eosio::check(rc == expected_rc, "return code does not match");

         std::vector<char> actual_result(result.data(), result.data() + result.size());
         eosio::check(actual_result == expected_result, "Result does not match");
      }
};
