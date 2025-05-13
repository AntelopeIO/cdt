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
    *  This is the packed representation of a call
    *
    *  @ingroup call
    */
   struct call {
      /**
       *  Name of the account the call is intended for
       */
      const name               receiver{};

      /**
       *  indicating if the call is read only or not
       */
      const execution_mode     exec_mode = execution_mode::read_write;

      /**
       *  if the receiver contract does not have sync_call entry point or its signature
       *  is invalid, when on_call_not_supported_mode is set to no_op,
       *  the sync call is no op, otherwise the call is aborted and an exception is raised.
       */
      const on_call_not_supported_mode not_supported_mode = on_call_not_supported_mode::abort;

      /**
       *  Payload data
       */
      const std::vector<char>  data{};

      /**
       * Construct a new call object with receiver, name, and payload data
       *
       * @tparam T  - Type of call data, must be serializable by `pack(...)`
       * @param receiver -  The name of the account this call is intended for
       * @param flags - The flags
       * @param payload - The call data that will be serialized via pack into data
       */
      template<typename T>
      call( struct name receiver, T&& payload, execution_mode exec_mode = execution_mode::read_write, on_call_not_supported_mode not_supported_mode = on_call_not_supported_mode::abort)
      : receiver(receiver)
      , exec_mode(exec_mode)
      , not_supported_mode(not_supported_mode)
      , data(pack(std::forward<T>(payload))) {}

      /// @cond INTERNAL
      EOSLIB_SERIALIZE( call, (receiver)(exec_mode)(not_supported_mode)(data) )
      /// @endcond

      /**
       * Make a call using the functor operator
       */
      int64_t operator()() const {
         uint64_t flags = (exec_mode == execution_mode::read_only) ? 0x01 : 0x00; // last bit indicating read only
         auto retval =  internal_use_do_not_use::call(receiver.value, flags, data.data(), data.size());

         if (retval == -1) {  // sync call is not supported by the receiver contract
            check(not_supported_mode == on_call_not_supported_mode::no_op, "receiver does not support sync call but on_call_not_supported_mode is set to abort");
         }
         return retval;
      }
   };

   /**
    * Wrapper for a call object.
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
   template <eosio::name::raw Func_Name, auto Func_Ref>
   struct call_wrapper {
      template <typename Receiver>
      constexpr call_wrapper(Receiver&& receiver, execution_mode exec_mode = execution_mode::read_write, on_call_not_supported_mode not_supported_mode = on_call_not_supported_mode::abort)
         : receiver(std::forward<Receiver>(receiver))
         , exec_mode(exec_mode)
         , not_supported_mode(not_supported_mode)
      {}

      static constexpr eosio::name func_name = eosio::name(Func_Name);
      eosio::name receiver {};
      execution_mode exec_mode = execution_mode::read_write;
      on_call_not_supported_mode not_supported_mode = on_call_not_supported_mode::abort;

      template <typename... Args>
      call to_call(Args&&... args)const {
         static_assert(detail::type_check<Func_Ref, Args...>());
         return call(receiver, std::make_tuple(func_name, detail::deduced<Func_Ref>{std::forward<Args>(args)...}), exec_mode, not_supported_mode);
      }

      using Return_Type = typename detail::function_traits<decltype(Func_Ref)>::return_type;

      template <typename... Args>
      Return_Type operator()(Args&&... args)const {
         auto size = to_call(std::forward<Args>(args)...)();

         if constexpr (std::is_void<Return_Type>::value) {
            return;
         } else {
            constexpr size_t max_stack_buffer_size = 512;
            char* buffer = (char*)(max_stack_buffer_size < size ? malloc(size) : alloca(size));
            internal_use_do_not_use::get_call_return_value(buffer, size);
            return unpack<Return_Type>(buffer, size);
         }
      }

   };
} // namespace eosio
