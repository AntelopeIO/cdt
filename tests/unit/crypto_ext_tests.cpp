/**
 *  @file
 *  @copyright defined in eosio.cdt/LICENSE.txt
 */

#include <eosio/tester.hpp>
#include <eosio/crypto_ext.hpp>

using namespace eosio::native;

// Definitions in `eosio.cdt/libraries/eosio/crypto_ext.hpp`
EOSIO_TEST_BEGIN(point_view_test)
   // -----------------------------------------------------
   std::string x_str = "0123456789abcdeffedcba9876543210";
   std::string y_str = "fedcba98765432100123456789abcdef";
   std::string serialized_str = "0123456789abcdeffedcba9876543210fedcba98765432100123456789abcdef";
   std::vector<char> x( x_str.begin(), x_str.end() );
   std::vector<char> y( y_str.begin(), y_str.end() );
   std::vector<char> serialized( serialized_str.begin(), serialized_str.end() );

   eosio::ec_point point{x, y};
   auto point_serialized = point.serialized();
   CHECK_EQUAL( serialized, point_serialized );

   eosio::ec_point_view  view{x, y};
   auto view_serialized = view.serialized();
   CHECK_EQUAL( serialized, view_serialized );
EOSIO_TEST_END

EOSIO_TEST_BEGIN(g1_point_view_test)
   // -----------------------------------------------------
   // g1_point_view(std::vector<char> x_, std::vector<char> y_)
   std::vector<char> chars_1(1), chars_32(32), chars_33(33), chars_64(64), chars_65(65);

   CHECK_EQUAL( (eosio::g1_point_view{chars_32, chars_32}.size), 32 );

   CHECK_ASSERT( "point size must match", ([&]() {eosio::g1_point_view{chars_1, chars_1};}) );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g1_point_view{chars_33, chars_33};}) );
   CHECK_ASSERT( "x's size must be equal to y's", ([&]() {eosio::g1_point_view{chars_32, chars_33};}) );

   // -----------------------------------------------------
   // g1_point_view(std::vector<char>& p)
   CHECK_EQUAL( (eosio::g1_point_view{chars_64}.size), 32 );

   CHECK_ASSERT( "point size must match", ([&]() {eosio::g1_point_view{chars_65};}) );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g1_point_view{chars_32};}) );
EOSIO_TEST_END

EOSIO_TEST_BEGIN(g2_point_view_test)
   // -----------------------------------------------------
   // g1_point_view(std::vector<char> x_, std::vector<char> y_)
   std::vector<char> chars_1(1), chars_64(64), chars_65(65), chars_128(128), chars_129(129);

   CHECK_EQUAL( (eosio::g2_point_view{chars_64, chars_64}.size), 64 );

   CHECK_ASSERT( "point size must match", ([&]() {eosio::g2_point_view{chars_1, chars_1};}) );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g2_point_view{chars_65, chars_65};}) );
   CHECK_ASSERT( "x's size must be equal to y's", ([&]() {eosio::g2_point_view{chars_64, chars_65};}) );

   // -----------------------------------------------------
   // g2_point_view(std::vector<char>& p)
   CHECK_EQUAL( (eosio::g2_point_view{chars_128}.size), 64 );

   CHECK_ASSERT( "point size must match", ([&]() {eosio::g2_point_view{chars_129};}) );
   CHECK_ASSERT( "point size must match", ([&]() {eosio::g2_point_view{chars_64};}) );
EOSIO_TEST_END

EOSIO_TEST_BEGIN(bigint_test)
   // -----------------------------------------------------
   // bigint(std::vector<char>& s)
   std::vector<char> chars_128(128), chars_256(256);

   CHECK_EQUAL( (eosio::bigint{chars_128}.size()), 128 );
   CHECK_EQUAL( (eosio::bigint{chars_256}.size()), 256 );
EOSIO_TEST_END

int main(int argc, char* argv[]) {
   bool verbose = false;
   if( argc >= 2 && std::strcmp( argv[1], "-v" ) == 0 ) {
      verbose = true;
   }
   silence_output(!verbose);

   EOSIO_TEST(point_view_test)
   EOSIO_TEST(g1_point_view_test)
   EOSIO_TEST(g2_point_view_test)
   EOSIO_TEST(bigint_test)

   return has_failed();
}
