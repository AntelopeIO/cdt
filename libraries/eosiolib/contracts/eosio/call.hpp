/**
 *  @file
 */
#pragma once
#include <cstdlib>
#include <type_traits>

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
      const bool               read_only = false;

      /**
       *  if the receiver contract does not have sync_call entry point or its signature
       *  is invalid, when no_op_if_receiver_no_support_sync_call is set to true,
       *  the sync call is no op, otherwise the call is aborted and an exception is raised.
       */
      const bool               no_op_if_receiver_no_support_sync_call = false;

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
      call( struct name receiver, T&& payload, bool read_only = false, bool no_op = false )
      : receiver(receiver)
      , read_only(read_only)
      , no_op_if_receiver_no_support_sync_call(no_op)
      , data(pack(std::forward<T>(payload))) {}

      /// @cond INTERNAL
      EOSLIB_SERIALIZE( call, (receiver)(read_only)(no_op_if_receiver_no_support_sync_call)(data) )
      /// @endcond

      /**
       * Make a call using the functor operator
       */
      int64_t operator()() const {
         uint64_t flags = read_only ? 0x01 : 0x00; // last bit indicating read only
         auto retval =  internal_use_do_not_use::call(receiver.value, flags, data.data(), data.size());

         if (retval == -1) {  // sync call is not supported by the receiver contract
            check(no_op_if_receiver_no_support_sync_call, "receiver does not support sync call but no_op_if_receiver_no_support_sync_call flag is not set");
         }
         return retval;
      }
   };

} // namespace eosio
