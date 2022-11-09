/**
 *  @file
 *  @copyright defined in eosio.cdt/LICENSE.txt
 */

#include <cstdint>
#include <cstdio>
#include <string>

#include <eosio/tester.hpp>
#include <eosio/crt.hpp>

using eosio::cdt::output_stream;

EOSIO_TEST_BEGIN(output_stream_push)
   std_err.clear();
   const char* msg = "abc";
   _prints(msg, eosio::cdt::output_stream_kind::std_err);
   CHECK_EQUAL(std_err.to_string(), "abc");
   CHECK_EQUAL(std_err.index(), 3);

   std_err.clear();
   const char* msg2 = "";
   _prints(msg2, eosio::cdt::output_stream_kind::std_err);
   CHECK_EQUAL(std_err.to_string(), "");
   CHECK_EQUAL(std_err.index(), 0);

   std_err.clear();
EOSIO_TEST_END

EOSIO_TEST_BEGIN(output_stream_push_overflow)
   std_err.clear();
   const auto initial_capacity = std_err.to_string().capacity();
   CHECK_EQUAL(std_err.index(), 0);

   std::string large_msg('x', initial_capacity + 1);

   _prints(large_msg.c_str(), eosio::cdt::output_stream_kind::std_err);
   CHECK_EQUAL(std_err.to_string().capacity() > initial_capacity, true);
   CHECK_EQUAL(std_err.index(), large_msg.size());

   std_err.clear();
EOSIO_TEST_END

EOSIO_TEST_BEGIN(output_stream_get_and_push)
   std_err.clear();
   std::string example_msg("abcdef");

   _prints(example_msg.c_str(), eosio::cdt::output_stream_kind::std_err);
   CHECK_EQUAL(strcmp(std_err.get(), "abcdef") , 0);

   std_err.push('g');
   CHECK_EQUAL(strcmp(std_err.get(), "abcdefg") , 0);

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
   EOSIO_TEST(output_stream_get_and_push)
   return has_failed();
}
