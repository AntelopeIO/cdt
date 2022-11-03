/**
 *  @file
 *  @copyright defined in eosio.cdt/LICENSE.txt
 */
#include <cstdint>
#include <string>

#include <eosio/tester.hpp>
#include <eosio/crt.hpp>

#include <cstdio>

using eosio::cdt::output_stream;

EOSIO_TEST_BEGIN(output_stream_push)
   std_err.clear();
   const char* msg = "abc";
   _prints(msg, eosio::cdt::output_stream_kind::std_err);
   CHECK_EQUAL(std_err.to_string(), "abc");

   std_err.clear();
   const char* msg2 = "";
   _prints(msg2, eosio::cdt::output_stream_kind::std_err);
   CHECK_EQUAL(std_err.to_string(), "");

   std_err.clear();
EOSIO_TEST_END

EOSIO_TEST_BEGIN(output_stream_push_overflow)
   std_err.clear();
   const auto max_size = std_err.max_size;

   char msg[max_size+1] = {};
   for (auto i = 0; i< max_size; ++i)
   msg[i] = 'x';

   _prints(msg, eosio::cdt::output_stream_kind::std_err);
   CHECK_EQUAL(std_err.to_string().size(), max_size - 1);

   std_err.clear();
EOSIO_TEST_END

int main(int argc, char* argv[]) {
   bool verbose = false;
   if( argc >= 2 && std::strcmp( argv[1], "-v" ) == 0 ) {
      verbose = true;
   }
   silence_output(!verbose);

   EOSIO_TEST(output_stream_push)
   EOSIO_TEST(output_stream_push_overflow)
   return has_failed();
}
