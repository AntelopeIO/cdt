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
   // std::vector<char> serialized() const
   std::vector<char> x {'0', '1', '2', '3', '4', '5', '6', '7'};
   std::vector<char> y {'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
   std::vector<char> expected {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

   eosio::point_view point{x, y};

   auto serialized = point.serialized();
   CHECK_EQUAL( expected, serialized );

   std::vector<char> x1(8, '\0');
   std::vector<char> y1(8, '\0');
   eosio::point_view point1{x1, y1};

   point1.copy_from(serialized);

   std::vector<char> point1_x(point1.x, point1.x + point1.size);
   std::vector<char> point1_y(point1.y, point1.y + point1.size);
   CHECK_EQUAL( point1_x, x );
   CHECK_EQUAL( point1_y, y );
EOSIO_TEST_END

EOSIO_TEST_BEGIN(g1_view_test)
   // -----------------------------------------------------
   // g1_view(std::vector<char> x_, std::vector<char> y_)
   std::vector<char> chars_1(1), chars_32(32), chars_33(33), chars_64(64), chars_65(65);

   CHECK_EQUAL( (eosio::g1_view{chars_32, chars_32}.size), 32 );

   CHECK_ASSERT( "G1 coordinate size must be 32", ([&]() {eosio::g1_view{chars_1, chars_1};}) );
   CHECK_ASSERT( "G1 coordinate size must be 32", ([&]() {eosio::g1_view{chars_33, chars_33};}) );
   CHECK_ASSERT( "x's size must be equal to y's", ([&]() {eosio::g1_view{chars_32, chars_33};}) );

   // -----------------------------------------------------
   // g1_view(std::vector<char>& p)
   CHECK_EQUAL( (eosio::g1_view{chars_64}.size), 32 );

   CHECK_ASSERT( "G1 serialized size must be 64", ([&]() {eosio::g1_view{chars_65};}) );
   CHECK_ASSERT( "G1 serialized size must be 64", ([&]() {eosio::g1_view{chars_32};}) );
EOSIO_TEST_END

EOSIO_TEST_BEGIN(g2_view_test)
   // -----------------------------------------------------
   // g1_view(std::vector<char> x_, std::vector<char> y_)
   std::vector<char> chars_1(1), chars_64(64), chars_65(65), chars_128(128), chars_129(129);

   CHECK_EQUAL( (eosio::g2_view{chars_64, chars_64}.size), 64 );

   CHECK_ASSERT( "G2 coordinate size must be 64", ([&]() {eosio::g2_view{chars_1, chars_1};}) );
   CHECK_ASSERT( "G2 coordinate size must be 64", ([&]() {eosio::g2_view{chars_65, chars_65};}) );
   CHECK_ASSERT( "x's size must be equal to y's", ([&]() {eosio::g2_view{chars_64, chars_65};}) );

   // -----------------------------------------------------
   // g2_view(std::vector<char>& p)
   CHECK_EQUAL( (eosio::g2_view{chars_128}.size), 64 );

   CHECK_ASSERT( "G2 serialized size must be 128", ([&]() {eosio::g2_view{chars_129};}) );
   CHECK_ASSERT( "G2 serialized size must be 128", ([&]() {eosio::g2_view{chars_64};}) );
EOSIO_TEST_END

EOSIO_TEST_BEGIN(bigint_test)
   // -----------------------------------------------------
   // bigint(std::vector<char>& s)
   std::vector<char> chars_128(128), chars_256(256);

   CHECK_EQUAL( (eosio::bigint{chars_128}.size), 128 );
   CHECK_EQUAL( (eosio::bigint{chars_256}.size), 256 );
EOSIO_TEST_END

int main(int argc, char* argv[]) {
   bool verbose = false;
   if( argc >= 2 && std::strcmp( argv[1], "-v" ) == 0 ) {
      verbose = true;
   }
   silence_output(!verbose);

   EOSIO_TEST(point_view_test)
   EOSIO_TEST(g1_view_test)
   EOSIO_TEST(g2_view_test)
   EOSIO_TEST(bigint_test)

   return has_failed();
}
