#include <eosio/name.hpp>
#include <eosio/action.hpp>
#include "native/eosio/intrinsics.hpp"
#include "native/eosio/crt.hpp"
#include <cstdint>
#include <functional>
#include <stdio.h>
#include <setjmp.h>

eosio::cdt::output_stream std_out;
eosio::cdt::output_stream std_err;

extern "C" {
   int main(int, char**);
   char* _mmap();

   static jmp_buf env;
   static jmp_buf test_env;
   static volatile int jmp_ret;
   jmp_buf* ___env_ptr = &env;
   char* ___heap;
   char* ___heap_ptr;
   char* ___heap_base_ptr;
   size_t ___pages;
   void ___putc(char c);
   bool ___disable_output;
   bool ___has_failed;
   bool ___earlier_unit_test_has_failed;

   void* __get_heap_base() {
      return ___heap_base_ptr;
   }

   size_t _current_memory() {
      return ___pages;
   }

   size_t _grow_memory(size_t size) {
      if ((___heap_ptr + (size*64*1024)) > (___heap_ptr + 100*1024*1024))
         eosio_assert(false, "__builtin_wasm_grow_memory");
      ___heap_ptr += (size*64*1024);
      return ++___pages;
   }

   void _prints_l(const char* cstr, uint32_t len, uint8_t which) {
      for (int i=0; i < len; i++) {
         if (which == eosio::cdt::output_stream_kind::std_out)
            std_out.push(cstr[i]);
         else if (which == eosio::cdt::output_stream_kind::std_err)
            std_err.push(cstr[i]);
         if (!___disable_output)
            ___putc(cstr[i]);
      }
   }

   void _prints(const char* cstr, uint8_t which) {
      for (int i=0; cstr[i] != '\0'; i++) {
         if (which == eosio::cdt::output_stream_kind::std_out)
            std_out.push(cstr[i]);
         else if (which == eosio::cdt::output_stream_kind::std_err)
            std_err.push(cstr[i]);
         if (!___disable_output)
            ___putc(cstr[i]);
      }
   }

   jmp_buf* __get_jmp_buf() {
      return ___env_ptr;
   }
   void __set_env_test() {
      ___env_ptr = &test_env;
   }
   void __reset_env() {
      ___env_ptr = &env;
   }

   int _wrap_main(int argc, char** argv) {
      using namespace eosio::native;
      int ret_val = 0;
      ___heap = _mmap();
      ___heap_ptr = ___heap;
      ___heap_base_ptr = ___heap;
      ___pages = 1;
      ___disable_output = false;
      ___has_failed = false;
      ___earlier_unit_test_has_failed = false;

      set_print_intrinsics();

      jmp_ret = setjmp(env);
      if (jmp_ret == 0) {
         ret_val = main(argc, argv);
      } else {
         ret_val = -1;
      }
      return ret_val;
   }

   extern "C" void* memset(void*, int, size_t);
   extern "C" void __bzero(void* to, size_t cnt) {
      char* cp{static_cast<char*>(to)};
      while (cnt--) *cp++ = 0;
   }
}
