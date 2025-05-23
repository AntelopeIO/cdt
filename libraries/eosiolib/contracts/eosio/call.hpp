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

   // Indicate whether a sync call is read_write or read_only. Default is read_write
   enum class access_mode { read_write = 0, read_only = 1 };

   // Indicate the action to take if the receiver does not support sync calls.
   // Default is abort_op
   enum class support_mode { abort_op = 0, no_op = 1 };

   // For a void function, when support_mode is set to no_op, the call_wrapper.
   // returns `std::optional<void_call>`. If the optional has no value, it indicates
   // the call was op-op; if the optional has a value of `void_call`, the call
   // executed successfully.
   struct void_call {
   };

   struct call_data_header {
      uint32_t version   = 0;
      uint64_t func_name = 0;

      EOSLIB_SERIALIZE(call_data_header, (version)(func_name))
   };

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
   template <eosio::name::raw Func_Name, auto Func_Ref, access_mode Access_Mode=access_mode::read_write, support_mode Support_Mode = support_mode::abort_op>
   struct call_wrapper {
      template <typename Receiver>
      constexpr call_wrapper(Receiver&& receiver)
         : receiver(std::forward<Receiver>(receiver))
      {}

      static constexpr eosio::name function_name = eosio::name(Func_Name);
      eosio::name receiver {};

      using orig_ret_type = typename detail::function_traits<decltype(Func_Ref)>::return_type;

      using return_type = std::conditional_t<
         Support_Mode == support_mode::abort_op,// if Support_Mode is abort_op
         orig_ret_type,                         // use the original return type
         std::conditional_t<                    // else
            std::is_void<orig_ret_type>::value, // original return type is void
            std::optional<void_call>,           // use optional of empty struct
            std::optional<orig_ret_type>        // use optional of original return type
         >
      >;

      template <typename... Args>
      return_type operator()(Args&&... args)const {
         static_assert(detail::type_check<Func_Ref, Args...>());

         uint64_t flags = 0x00;
         if constexpr (Access_Mode == access_mode::read_only) {
            flags = 0x01;
         }

         call_data_header header{ .version   = 0,
                                  .func_name = function_name.value };
 
         const std::vector<char> data{ pack(std::forward_as_tuple(header, detail::deduced<Func_Ref>{std::forward<Args>(args)...})) };

         auto ret_val_size = internal_use_do_not_use::call(receiver.value, flags, data.data(), data.size());

         if (ret_val_size < 0) {  // the receiver does not support sync calls
            if constexpr (Support_Mode == support_mode::abort_op) {
               check(false, "receiver does not support sync call but support_mode is set to abort_op");
            } else {
               return std::nullopt;
            }
         }

         // The sync call has been executed by the receiver
         if constexpr (std::is_void<return_type>::value) {
            return;
         } else {
            if constexpr (Support_Mode == support_mode::no_op && std::is_void<orig_ret_type>::value) {
               return void_call{};
            } else {
               constexpr size_t max_stack_buffer_size = 512;
               char* buffer = (char*)(max_stack_buffer_size < ret_val_size ? malloc(ret_val_size) : alloca(ret_val_size)); // intentionally no `free()` is called. the memory will be reset after execution
               internal_use_do_not_use::get_call_return_value(buffer, ret_val_size);
               auto ret_val = unpack<orig_ret_type>(buffer, ret_val_size);

               if constexpr (Support_Mode == support_mode::no_op) {
                  return std::make_optional(ret_val);
               } else {
                  return ret_val;
               }
            }
         }
      }
   };
} // namespace eosio
