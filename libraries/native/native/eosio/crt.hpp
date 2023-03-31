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

namespace eosio { namespace native {
   void set_print_intrinsics();
}} //eosio::native

extern eosio::cdt::output_stream std_out;
extern eosio::cdt::output_stream std_err;

extern "C" {
   extern jmp_buf* ___env_ptr;
   extern char*    ___heap_ptr;
   extern bool ___disable_output;
   extern bool ___has_failed;
   extern bool ___earlier_unit_test_has_failed;

   jmp_buf* __get_jmp_buf();
   void __set_env_test();
   void __reset_env();
   void _prints_l(const char* cstr, uint32_t len, uint8_t which);
   void _prints(const char* cstr, uint8_t which);
   
   //used only in shared object
   void register_intrinsic(int64_t, void(*)());
   void initialize();
}
