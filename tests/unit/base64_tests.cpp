/**
 *  @file
 *  @copyright defined in eosio.cdt/LICENSE.txt
 */

#include <eosio/tester.hpp>
#include <eosio/base64.hpp>

using namespace eosio;
using namespace std::literals;

EOSIO_TEST_BEGIN(base64enc)
   auto input = "abc123$&()'?\xb4\xf5\x01\xfa~a"s;
   auto expected_output = "YWJjMTIzJCYoKSc/tPUB+n5h"s;

   CHECK_EQUAL(expected_output, base64_encode(input));
EOSIO_TEST_END

EOSIO_TEST_BEGIN(base64urlenc)
   auto input = "abc123$&()'?\xb4\xf5\x01\xfa~a"s;
   auto expected_output = "YWJjMTIzJCYoKSc_tPUB-n5h"s;

   CHECK_EQUAL(expected_output, base64url_encode(input));
EOSIO_TEST_END

EOSIO_TEST_BEGIN(base64dec)
   auto input = "YWJjMTIzJCYoKSc/tPUB+n5h"s;
   auto expected_output = "abc123$&()'?\xb4\xf5\x01\xfa~a"s;

   CHECK_EQUAL(expected_output, base64_decode(input));
EOSIO_TEST_END

EOSIO_TEST_BEGIN(base64urldec)
   auto input = "YWJjMTIzJCYoKSc_tPUB-n5h"s;
   auto expected_output = "abc123$&()'?\xb4\xf5\x01\xfa~a"s;

   CHECK_EQUAL(expected_output, base64url_decode(input));
EOSIO_TEST_END

EOSIO_TEST_BEGIN(base64dec_extraequals)
   CHECK_ASSERT( "encountered non-base64 character",
      ([](){
         base64_decode("YWJjMTIzJCYoKSc/tPUB+n5h========="s);
      }));
EOSIO_TEST_END

EOSIO_TEST_BEGIN(base64dec_bad_stuff)
   CHECK_ASSERT( "encountered non-base64 character",
      ([](){
         base64_decode("YWJjMTIzJCYoKSc/tPU$B+n5h="s);
      }));
EOSIO_TEST_END

// tests from https://github.com/ReneNyffenegger/cpp-base64/blob/master/test.cpp
EOSIO_TEST_BEGIN(base64_cpp_base64_tests)
   //
   // Note: this file must be encoded in UTF-8
   // for the following test, otherwise, the test item
   // fails.
   //
   const std::string orig =
      "René Nyffenegger\n"
      "http://www.renenyffenegger.ch\n"
      "passion for data\n";

   std::string encoded = base64_encode({orig.c_str(), orig.length()});
   std::string decoded = base64_decode(encoded);

   CHECK_EQUAL(encoded, "UmVuw6kgTnlmZmVuZWdnZXIKaHR0cDovL3d3dy5yZW5lbnlmZmVuZWdnZXIuY2gKcGFzc2lvbiBmb3IgZGF0YQo=");
   CHECK_EQUAL(decoded, orig);

   // Test all possibilites of fill bytes (none, one =, two ==)
   // References calculated with: https://www.base64encode.org/

   std::string rest0_original = "abc";
   std::string rest0_reference = "YWJj";

   std::string rest0_encoded = base64_encode({rest0_original.c_str(),rest0_original.length()});
   std::string rest0_decoded = base64_decode(rest0_encoded);

   CHECK_EQUAL(rest0_decoded, rest0_original);
   CHECK_EQUAL(rest0_reference, rest0_encoded);

   std::string rest1_original = "abcd";
   std::string rest1_reference = "YWJjZA==";

   std::string rest1_encoded = base64_encode({rest1_original.c_str(), rest1_original.length()});
   std::string rest1_decoded = base64_decode(rest1_encoded);

   CHECK_EQUAL(rest1_decoded, rest1_original);
   CHECK_EQUAL(rest1_reference, rest1_encoded);

   std::string rest2_original = "abcde";
   std::string rest2_reference = "YWJjZGU=";

   std::string rest2_encoded = base64_encode({rest2_original.c_str(),rest2_original.length()});
   std::string rest2_decoded = base64_decode(rest2_encoded);

   CHECK_EQUAL(rest2_decoded, rest2_original);
   CHECK_EQUAL(rest2_reference, rest2_encoded);

   // --------------------------------------------------------------
   //
   // Data that is 17 bytes long requires one padding byte when
   // base-64 encoded. Such an encoded string could not correctly
   // be decoded when encoded with «url semantics». This bug
   // was discovered by https://github.com/kosniaz. The following
   // test checks if this bug was fixed:
   //
   std::string a17_orig    = "aaaaaaaaaaaaaaaaa";
   std::string a17_encoded     = base64_encode(a17_orig);
   std::string a17_encoded_url = base64url_encode(a17_orig);

   CHECK_EQUAL(a17_encoded, "YWFhYWFhYWFhYWFhYWFhYWE=");
   CHECK_EQUAL(a17_encoded_url, "YWFhYWFhYWFhYWFhYWFhYWE.");
   CHECK_EQUAL(base64_decode(a17_encoded_url), a17_orig);
   CHECK_EQUAL(base64_decode(a17_encoded), a17_orig);

   // --------------------------------------------------------------

   // characters 63 and 64 / URL encoding

   std::string s_6364 = "\x03" "\xef" "\xff" "\xf9";

   std::string s_6364_encoded     = base64_encode(s_6364);
   std::string s_6364_encoded_url = base64url_encode(s_6364);

   CHECK_EQUAL(s_6364_encoded, "A+//+Q==");
   CHECK_EQUAL(s_6364_encoded_url, "A-__-Q..");
   CHECK_EQUAL(base64_decode(s_6364_encoded), s_6364);
   CHECK_EQUAL(base64_decode(s_6364_encoded_url), s_6364);

   // ----------------------------------------------

   std::string unpadded_input   = "YWJjZGVmZw"; // Note the 'missing' "=="
   std::string unpadded_decoded = base64_decode(unpadded_input);
   CHECK_EQUAL(unpadded_decoded, "abcdefg");

   unpadded_input   = "YWJjZGU"; // Note the 'missing' "="
   unpadded_decoded = base64_decode(unpadded_input);
   CHECK_EQUAL(unpadded_decoded, "abcde");

   unpadded_input   = "";
   unpadded_decoded = base64_decode(unpadded_input);
   CHECK_EQUAL(unpadded_decoded, "");

   unpadded_input   = "YQ";
   unpadded_decoded = base64_decode(unpadded_input);
   CHECK_EQUAL(unpadded_decoded, "a");

   unpadded_input   = "YWI";
   unpadded_decoded = base64_decode(unpadded_input);
   CHECK_EQUAL(unpadded_decoded, "ab");

   CHECK_ASSERT( "wrong encoded string size",
      ([](){
         std::string not_null_terminated = std::string(1, 'a');
         base64_decode(not_null_terminated);
      }));
   // --------------------------------------------------------------
   //
   // Test the string_view interface (which required C++17)
   //
   std::string_view sv_orig    = "foobarbaz";
   std::string sv_encoded = base64_encode(sv_orig);

   CHECK_EQUAL(sv_encoded, "Zm9vYmFyYmF6");

   std::string sv_decoded = base64_decode(sv_encoded);

   CHECK_EQUAL(sv_decoded, sv_orig);

EOSIO_TEST_END

int main(int argc, char* argv[]) {
   bool verbose = false;
   if( argc >= 2 && std::strcmp( argv[1], "-v" ) == 0 ) {
      verbose = true;
   }
   silence_output(!verbose);

   EOSIO_TEST(base64enc);
   EOSIO_TEST(base64urlenc);
   EOSIO_TEST(base64dec);
   EOSIO_TEST(base64urldec);
   EOSIO_TEST(base64dec_extraequals);
   EOSIO_TEST(base64dec_bad_stuff);
   EOSIO_TEST(base64_cpp_base64_tests);
   return has_failed();
}
