/**
 *  @file
 */
#pragma once
#include <cstdlib>
#include <type_traits>

#include "detail.hpp"
#include "../../core/eosio/serialize.hpp"
#include "../../core/eosio/datastream.hpp"
#include "../../core/eosio/name.hpp"

namespace eosio {

   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         int64_t call(uint64_t receiver, uint64_t flags, const char* data, size_t data_size);

         __attribute__((eosio_wasm_import))
         uint32_t get_call_return_value( void* mem, uint32_t len );

         __attribute__((eosio_wasm_import))
         uint32_t get_call_data( void* mem, uint32_t len );

         __attribute__((eosio_wasm_import))
         void set_call_return_value( void* mem, uint32_t len );
      }
   };

   /**
    *  @defgroup call call
    *  @ingroup contracts
    *  @brief Defines type-safe C++ wrappers for querying call and sending call
    *  @note There are some methods from the @ref call that can be used directly from C++
    */

   inline int64_t call(uint64_t receiver, uint64_t flags, const char* data, size_t data_size) {
      return internal_use_do_not_use::call(receiver, flags, data, data_size);
   }

   inline uint32_t get_call_return_value( void* mem, uint32_t len ) {
     return internal_use_do_not_use::get_call_return_value(mem, len);
   }

   inline uint32_t get_call_data( void* mem, uint32_t len ) {
     return internal_use_do_not_use::get_call_data(mem, len);
   }

   inline void set_call_return_value( void* mem, uint32_t len ) {
     internal_use_do_not_use::set_call_return_value(mem, len);
   }

   // Request a sync call is read_write or read_only. Default is read_write
   enum execution_mode { read_write = 0, read_only = 1 };

   // Behaviour of a sync call if the receiver does not support sync calls
   // Default is abort
   enum on_call_not_supported_mode { abort = 0, no_op = 1 };

   /**
    * Wrapper for simplifying making a sync call
    *
    * @brief Used to wrap an a particular sync call to simplify the process of other contracts making sync calls to the "wrapped" call.
    * Example:
    * @code
    * // defined by contract writer of the sync call functions
    * using get_func = call_wrapper<"get"_n, &callee::get, uint32_t>;
    * // usage by different contract writer
    * get_func{"callee"_n}();
    * // or
    * get_func get{"callee"_n};
    * get();
    * @endcode
    */
   template <eosio::name::raw Func_Name, auto Func_Ref, execution_mode Exec_Mode=execution_mode::read_write, on_call_not_supported_mode Not_Supported_Mode = on_call_not_supported_mode::abort>
   struct call_wrapper {
      template <typename Receiver>
      constexpr call_wrapper(Receiver&& receiver)
         : receiver(std::forward<Receiver>(receiver))
      {}

      static constexpr eosio::name func_name = eosio::name(Func_Name);
      eosio::name receiver {};

      using ret_type = typename detail::function_traits<decltype(Func_Ref)>::return_type;

      template <typename... Args>
      ret_type operator()(Args&&... args)const {
         uint64_t flags = 0x00;
         if constexpr (Exec_Mode == execution_mode::read_only) {
            flags = 0x01;
         }
         const std::vector<char> data{ pack(std::make_tuple(func_name, detail::deduced<Func_Ref>{std::forward<Args>(args)...})) };

         auto ret_val_size = internal_use_do_not_use::call(receiver.value, flags, data.data(), data.size());

         if (ret_val_size < 0) {
            if constexpr (Not_Supported_Mode == on_call_not_supported_mode::abort) {
               check(false, "receiver does not support sync call while on_call_not_supported_mode is set to abort");
            } else {
               if constexpr (std::is_void<ret_type>::value) {
                  return;
               } else if constexpr (std::is_default_constructible_v<ret_type>) {
                  return {};
               } else {
                  static_assert(std::is_default_constructible_v<ret_type>, "Return type of on_call_not_supported_mode::no_op function must be default constructible");
               }
            }
         }

         if constexpr (std::is_void<ret_type>::value) {
            return;
         } else {
            constexpr size_t max_stack_buffer_size = 512;
            char* buffer = (char*)(max_stack_buffer_size < ret_val_size ? malloc(ret_val_size) : alloca(ret_val_size)); // intentionally no `free()` is called. the memory will be reset after execution
            internal_use_do_not_use::get_call_return_value(buffer, ret_val_size);
            return unpack<ret_type>(buffer, ret_val_size);
         }
      }
   };
} // namespace eosio
