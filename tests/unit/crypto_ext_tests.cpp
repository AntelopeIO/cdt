/**
 *  @file
 *  @copyright defined in eosio.cdt/LICENSE.txt
 */

#include <eosio/tester.hpp>
#include <eosio/crypto_ext.hpp>
#include <eosio/crypto_bls_ext.hpp>

#include <cstring>

using namespace eosio::native;

// Definitions in `eosio.cdt/libraries/eosio/crypto_ext.hpp`
EOSIO_TEST_BEGIN(ec_point_test)
   std::string x_str = "0123456789abcdeffedcba9876543210";
   std::string y_str = "fedcba98765432100123456789abcdef";
   std::string serialized_str = "0123456789abcdeffedcba9876543210fedcba98765432100123456789abcdef";
   std::vector<char> x( x_str.begin(), x_str.end() );
   std::vector<char> y( y_str.begin(), y_str.end() );
   std::vector<char> serialized( serialized_str.begin(), serialized_str.end() );

   eosio::ec_point point{x, y};
   auto point_serialized = point.serialized();
   CHECK_EQUAL( serialized, point_serialized );

   eosio::ec_point point_from_serialized {point_serialized};
   CHECK_EQUAL( point_from_serialized.serialized(), point_serialized);

   eosio::ec_point_view view{x.data(), static_cast<uint32_t>(x.size()), y.data(), static_cast<uint32_t>(y.size())};
   auto view_serialized = view.serialized();
   CHECK_EQUAL( serialized, view_serialized );

   eosio::ec_point_view view1{ view_serialized };
   CHECK_EQUAL( view1.serialized(), view_serialized );

   eosio::ec_point_view view2{ point };
   CHECK_EQUAL( view2.serialized(), serialized );
EOSIO_TEST_END

EOSIO_TEST_BEGIN(g1_point_test)
   std::vector<char> chars_1(1), chars_32(32), chars_33(33), chars_64(64), chars_65(65);

   CHECK_EQUAL( (eosio::g1_point{chars_32, chars_32}.serialized().size()), 64 );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g1_point{chars_1, chars_1};}) );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g1_point{chars_33, chars_33};}) );
   CHECK_ASSERT( "x's size must be equal to y's", ([&]() {eosio::g1_point{chars_32, chars_33};}) );

   CHECK_EQUAL( (eosio::g1_point_view{chars_32.data(), static_cast<uint32_t>(chars_32.size()), chars_32.data(), static_cast<uint32_t>(chars_32.size())}.size), 32 );

   CHECK_ASSERT( "point size must match", ([&]() {eosio::g1_point_view{chars_1.data(), static_cast<uint32_t>(chars_1.size()), chars_1.data(), static_cast<uint32_t>(chars_1.size())};}) );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g1_point_view{chars_33.data(), static_cast<uint32_t>(chars_33.size()), chars_33.data(), static_cast<uint32_t>(chars_33.size())};}) );
   CHECK_ASSERT( "x's size must be equal to y's", ([&]() {eosio::g1_point_view{chars_32.data(), static_cast<uint32_t>(chars_32.size()), chars_33.data(), static_cast<uint32_t>(chars_33.size())};}) );

   CHECK_EQUAL( (eosio::g1_point_view{chars_64}.size), 32 );

   CHECK_ASSERT( "point size must match", ([&]() {eosio::g1_point_view{chars_65};}) );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g1_point_view{chars_32};}) );
EOSIO_TEST_END

EOSIO_TEST_BEGIN(g2_point_test)
   std::vector<char> chars_1(1), chars_64(64), chars_65(65), chars_128(128), chars_129(129);

   CHECK_EQUAL( (eosio::g2_point{chars_64, chars_64}.serialized().size()), 128 );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g2_point{chars_1, chars_1};}) );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g2_point{chars_65, chars_65};}) );
   CHECK_ASSERT( "x's size must be equal to y's", ([&]() {eosio::g2_point{chars_64, chars_65};}) );

   CHECK_EQUAL( (eosio::g2_point_view{chars_64.data(), static_cast<uint32_t>(chars_64.size()), chars_64.data(), static_cast<uint32_t>(chars_64.size())}.size), 64 );

   CHECK_ASSERT( "point size must match", ([&]() {eosio::g2_point_view{chars_1.data(), static_cast<uint32_t>(chars_1.size()), chars_1.data(), static_cast<uint32_t>(chars_1.size())};}) );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g2_point_view{chars_65.data(), static_cast<uint32_t>(chars_65.size()), chars_65.data(), static_cast<uint32_t>(chars_65.size())};}) );
   CHECK_ASSERT( "x's size must be equal to y's", ([&]() {eosio::g2_point_view{chars_64.data(), static_cast<uint32_t>(chars_64.size()), chars_65.data(), static_cast<uint32_t>(chars_65.size())};}) );

   CHECK_EQUAL( (eosio::g2_point_view{chars_128}.size), 64 );

   CHECK_ASSERT( "point size must match", ([&]() {eosio::g2_point_view{chars_129};}) );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g2_point_view{chars_64};}) );
EOSIO_TEST_END

EOSIO_TEST_BEGIN(bigint_test)
   std::vector<char> chars_128(128), chars_256(256);

   CHECK_EQUAL( (eosio::bigint{chars_128}.size()), 128 );
   CHECK_EQUAL( (eosio::bigint{chars_256}.size()), 256 );
EOSIO_TEST_END

// Runs bls_pop_verify and bls_signature_verify against stub host intrinsics and pins two things:
// the native trampoline forwards 2 G1 points and 2 G2 points to bls_pairing, and a failed
// pairing makes both helpers return false whatever the result buffer holds.
EOSIO_TEST_BEGIN(bls_verify_pairing_contract_test)
   // g2_fromMessage reaches these before the pairing; their output does not matter here.
   intrinsics::set_intrinsic<intrinsics::bls_fp_mod>(
      []( const char*, uint32_t, char*, uint32_t ) -> int32_t { return 0; } );
   intrinsics::set_intrinsic<intrinsics::bls_g2_map>(
      []( const char*, uint32_t, char*, uint32_t ) -> int32_t { return 0; } );
   intrinsics::set_intrinsic<intrinsics::bls_g2_add>(
      []( const char*, uint32_t, const char*, uint32_t, char*, uint32_t ) -> int32_t { return 0; } );

   // Distinct fills so the stub can tell the operands apart by content, not only by length.
   eosio::bls_g1 pubkey{};
   eosio::bls_g2 proof{};
   pubkey.fill( '\xa5' );
   proof.fill( '\x5c' );

   constexpr uint32_t g1_size = static_cast<uint32_t>( std::tuple_size<eosio::bls_g1>::value );
   constexpr uint32_t g2_size = static_cast<uint32_t>( std::tuple_size<eosio::bls_g2>::value );
   constexpr uint32_t expected_g1_len = 2 * g1_size;
   constexpr uint32_t expected_g2_len = 2 * g2_size;

   uint32_t g1_len = 0;
   uint32_t g2_len = 0;
   uint32_t pairs  = 0;
   bool     g1_carries_pubkey = false;   // second G1 point is the public key
   bool     g2_carries_proof  = false;   // first G2 point is the signature proof

   // Success: the host writes GT_ONE and returns 0, so both helpers verify.
   intrinsics::set_intrinsic<intrinsics::bls_pairing>(
      [&]( const char* g1, uint32_t g1l, const char* g2, uint32_t g2l, uint32_t n, char* res, uint32_t res_len ) -> int32_t {
         g1_len = g1l;
         g2_len = g2l;
         pairs  = n;
         g1_carries_pubkey = g1 != nullptr && g1l == expected_g1_len
                             && std::memcmp( g1 + g1_size, pubkey.data(), g1_size ) == 0;
         g2_carries_proof  = g2 != nullptr && g2l == expected_g2_len
                             && std::memcmp( g2, proof.data(), g2_size ) == 0;
         if ( res != nullptr && res_len >= eosio::detail::GT_ONE.size() )
            std::memcpy( res, eosio::detail::GT_ONE.data(), eosio::detail::GT_ONE.size() );
         return 0;
      } );

   CHECK_EQUAL( eosio::bls_pop_verify( pubkey, proof ), true )
   CHECK_EQUAL( pairs, 2u )
   CHECK_EQUAL( g1_len, expected_g1_len )
   CHECK_EQUAL( g2_len, expected_g2_len )
   CHECK_EQUAL( g1_carries_pubkey, true )
   CHECK_EQUAL( g2_carries_proof, true )

   g1_len = g2_len = pairs = 0;
   g1_carries_pubkey = g2_carries_proof = false;
   CHECK_EQUAL( eosio::bls_signature_verify( pubkey, proof, "message" ), true )
   CHECK_EQUAL( pairs, 2u )
   CHECK_EQUAL( g1_len, expected_g1_len )
   CHECK_EQUAL( g2_len, expected_g2_len )
   CHECK_EQUAL( g1_carries_pubkey, true )
   CHECK_EQUAL( g2_carries_proof, true )

   // Failure: the host rejects an operand but still writes a result that would compare equal,
   // so these pass only if the return code is read.
   intrinsics::set_intrinsic<intrinsics::bls_pairing>(
      []( const char*, uint32_t, const char*, uint32_t, uint32_t, char* res, uint32_t res_len ) -> int32_t {
         if ( res != nullptr && res_len >= eosio::detail::GT_ONE.size() )
            std::memcpy( res, eosio::detail::GT_ONE.data(), eosio::detail::GT_ONE.size() );
         return -1;
      } );

   CHECK_EQUAL( eosio::bls_pop_verify( pubkey, proof ), false )
   CHECK_EQUAL( eosio::bls_signature_verify( pubkey, proof, "message" ), false )
EOSIO_TEST_END

int main(int argc, char* argv[]) {
   bool verbose = false;
   if( argc >= 2 && std::strcmp( argv[1], "-v" ) == 0 ) {
      verbose = true;
   }
   silence_output(!verbose);

   EOSIO_TEST(ec_point_test)
   EOSIO_TEST(g1_point_test)
   EOSIO_TEST(g2_point_test)
   EOSIO_TEST(bigint_test)
   EOSIO_TEST(bls_verify_pairing_contract_test)

   return has_failed();
}
