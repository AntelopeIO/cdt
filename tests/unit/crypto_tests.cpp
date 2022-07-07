/**
 *  @file
 *  @copyright defined in eosio.cdt/LICENSE.txt
 */

#include <eosio/tester.hpp>
#include <eosio/crypto.hpp>

using eosio::public_key;
using eosio::signature;
using namespace eosio::native;

// Definitions in `eosio.cdt/libraries/eosio/crypto.hpp`
EOSIO_TEST_BEGIN(public_key_type_test)
   // -----------------------------------------------------
   // bool operator==(const public_key&, const public_key&)
   CHECK_EQUAL( (public_key(std::in_place_index<0>, std::array<char, 33>{})  == public_key(std::in_place_index<0>, std::array<char, 33>{})), true  )
   CHECK_EQUAL( (public_key(std::in_place_index<0>, std::array<char, 33>{1}) == public_key(std::in_place_index<0>, std::array<char, 33>{})), false )

   // -----------------------------------------------------
   // bool operator!=(const public_key&, const public_key&)
   CHECK_EQUAL( (public_key(std::in_place_index<0>, std::array<char, 33>{})  != public_key(std::in_place_index<0>, std::array<char, 33>{})), false )
   CHECK_EQUAL( (public_key(std::in_place_index<0>, std::array<char, 33>{1}) != public_key(std::in_place_index<0>, std::array<char, 33>{})), true  )
EOSIO_TEST_END

// Definitions in `eosio.cdt/libraries/eosio/crypto.hpp`
EOSIO_TEST_BEGIN(signature_type_test)
   // ---------------------------------------------------
   // bool operator==(const signature&, const signature&)
   CHECK_EQUAL( (signature(std::in_place_index<0>, std::array<char, 65>{})  == signature(std::in_place_index<0>, std::array<char, 65>{})), true  )
   CHECK_EQUAL( (signature(std::in_place_index<0>, std::array<char, 65>{1}) == signature(std::in_place_index<0>, std::array<char, 65>{})), false )

   // ---------------------------------------------------
   // bool operator!=(const signature&, const signature&)
   CHECK_EQUAL( (signature(std::in_place_index<0>, std::array<char, 65>{1}) != signature(std::in_place_index<0>, std::array<char, 65>{})), true  )
   CHECK_EQUAL( (signature(std::in_place_index<0>, std::array<char, 65>{})  != signature(std::in_place_index<0>, std::array<char, 65>{})), false )
EOSIO_TEST_END

static inline constexpr uint8_t hex_char_to_bin(char c) {
   return (c <= 57) ? c - 48 : (c <= 70) ? (c-65) + 0x0a : (c-97) + 0x0a; 
}

static inline std::array<char, 32> to_bin(std::string h) {
   std::array<char, 32> bytes;
   for (int i=0, j=0; i < h.size(); i+=2, j++) {
      bytes[j] = (hex_char_to_bin(h[i])*16) + hex_char_to_bin(h[i+1]);
   }
   return bytes;
}

static inline void check_checksum256(const eosio::checksum256& h, std::string b) {
   const auto& harr = h.extract_as_byte_array();
   const auto& barr = to_bin(b);
   eosio::print_f("HARR % %\n", harr.size(), barr.size());
   CHECK_EQUAL(harr.size(), b.size())
   for (int i=0; i < b.size(); i++) {
      eosio::print_f("H : % | B : %\n", (int)harr[i], (int)b[i]);
      CHECK_EQUAL(harr[i], b[i]);
   }
}

// Definitions in `eosio.cdt/libraries/eosio/crypto.hpp`
EOSIO_TEST_BEGIN(sha3_tests)
   const char* empty = "";
   const char* abc   = "abc";
   const char* alpha = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
   {
      intrinsics::set_intrinsic<intrinsics::sha3>(
         [](const char* s, uint32_t sl, char* h, uint32_t h_l, int32_t k) {
            const char* res = "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470";
            std::memcpy(h, res, h_l);
      });

      auto hash = eosio::sha3(empty, strlen(empty));
      check_checksum256(hash, "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470");
   }
EOSIO_TEST_END

int main(int argc, char* argv[]) {
   bool verbose = false;
   if( argc >= 2 && std::strcmp( argv[1], "-v" ) == 0 ) {
      verbose = true;
   }
   silence_output(!verbose);

   EOSIO_TEST(public_key_type_test)
   EOSIO_TEST(signature_type_test)
   EOSIO_TEST(sha3_tests)
   return has_failed();
}
