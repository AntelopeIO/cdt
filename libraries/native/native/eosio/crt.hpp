#pragma once
#include <setjmp.h>

#include <string>

namespace eosio { namespace cdt {
   enum output_stream_kind {
      std_out,
      std_err,
      none
   };
   class output_stream {
      static constexpr size_t initial_size = 1024 * 4;
      std::string output;

      public:
      output_stream() { output.reserve(initial_size); }
      const std::string& to_string() const { return output; }
      const char* get() const { return output.c_str(); }
      size_t index() const { return output.size(); }
      void push(char c) { output.push_back(c); }
      void clear() { output.clear(); }
   };
}} //ns eosio::cdt

extern eosio::cdt::output_stream std_out;
extern eosio::cdt::output_stream std_err;
extern "C" jmp_buf* ___env_ptr;
extern "C" char*    ___heap_ptr;

extern "C" {
   void __set_env_test();
   void __reset_env();
   void _prints_l(const char* cstr, uint32_t len, uint8_t which);
   void _prints(const char* cstr, uint8_t which);
}
