/**
 *  @file
 *  @copyright defined in eosio.cdt/LICENSE.txt
 */

#include <string>
#include <limits>

//#include <eosio/eosio.hpp>
#include <eosio/hex_conversion.hpp>
#include <eosio/tester.hpp>

using std::string;
using eosio::check;
using namespace eosio::wideint;

EOSIO_TEST_BEGIN(hex_test)

   CHECK_ASSERT( "eosio::wideint::from_hex Empty hexadecimal string", [&]() {eosio::wideint::from_hex<uint128_t>("");} )
   CHECK_ASSERT( "eosio::wideint::from_hex Invalid hexadecimal string", [&]() {eosio::wideint::from_hex<uint128_t>("Z");} )
   CHECK_ASSERT( "eosio::wideint::from_hex Invalid hexadecimal string", [&]() {eosio::wideint::from_hex<uint128_t>("AB54A98CEB1F0AD2Z");} )

   CHECK_EQUAL( 0ULL, eosio::wideint::from_hex<uint128_t>("0") );
   CHECK_EQUAL( 0ULL, eosio::wideint::from_hex<uint128_t>("00000") );
   CHECK_EQUAL( 1ULL, eosio::wideint::from_hex<uint128_t>("0x001") );
   CHECK_EQUAL( 1ULL, eosio::wideint::from_hex<uint128_t>("1") );
   CHECK_EQUAL( 255ULL, eosio::wideint::from_hex<uint128_t>("FF") );
   CHECK_EQUAL( 256ULL, eosio::wideint::from_hex<uint128_t>("100") );
   CHECK_EQUAL( 12345678901234567890ULL, eosio::wideint::from_hex<uint128_t>("AB54A98CEB1F0AD2") );
   //CHECK_EQUAL( uint128_t{170141183460469231731687303715884105727}, eosio::wideint::from_hex<uint128_t>("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF") );
   //or
   //CHECK_EQUAL( 170141183460469231731687303715884105727uwb, eosio::wideint::from_hex<uint128_t>("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF") );

silence_output(false);

   CHECK_EQUAL( "0", eosio::wideint::to_hex<uint128_t>(0ULL) );
   CHECK_EQUAL( "1", eosio::wideint::to_hex<uint128_t>(1ULL) );
   CHECK_EQUAL( "ff", eosio::wideint::to_hex<uint128_t>(255ULL) );
   CHECK_EQUAL( "100", eosio::wideint::to_hex<uint128_t>(256ULL) );
   CHECK_EQUAL( "ab54a98ceb1f0ad2", eosio::wideint::to_hex<uint128_t>(12345678901234567890ULL) );
   //CHECK_EQUAL( "fffffffffffffffffffffffffffffff", eosio::wideint::to_hex<uint128_t>(uint128_t{170141183460469231731687303715884105727}) );
   //or
   //CHECK_EQUAL("fffffffffffffffffffffffffffffff", eosio::wideint::to_hex<uint128_t>( 170141183460469231731687303715884105727uwb) );
   

EOSIO_TEST_END

int main(int argc, char* argv[]) {
   bool verbose = false;
   if( argc >= 2 && std::strcmp( argv[1], "-v" ) == 0 ) {
      verbose = true;
   }
   silence_output(!verbose);

   EOSIO_TEST(hex_test);
   return has_failed();
}
