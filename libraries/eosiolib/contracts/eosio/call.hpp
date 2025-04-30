/**
 *  @file
 */
#pragma once
#include <cstdlib>
#include <type_traits>

#include "../../core/eosio/serialize.hpp"
#include "../../core/eosio/datastream.hpp"
#include "../../core/eosio/name.hpp"
#include "../../core/eosio/fixed_bytes.hpp"
#include "../../core/eosio/ignore.hpp"
#include "../../core/eosio/time.hpp"

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
      name               receiver;

      /**
       *  flags indicating call properties, like read only request
       */
      uint64_t           flags;

      /**
       *  Payload data
       */
      std::vector<char>  data;

      /**
       *  Default Constructor
       */
      call() = default;

      /**
       * Construct a new call object with receiver, name, and payload data
       *
       * @tparam T  - Type of call data, must be serializable by `pack(...)`
       * @param receiver -  The name of the account this call is intended for
       * @param flags - The flags
       * @param value - The call data that will be serialized via pack into data
       */
      template<typename T>
      call( struct name receiver, uint64_t flags, T&& value )
      :receiver(receiver), flags(flags), data(pack(std::forward<T>(value))) {}

      /// @cond INTERNAL
      EOSLIB_SERIALIZE( call, (receiver)(flags)(data) )
      /// @endcond

      /**
       * Make a call using the functor operator
       */
      int64_t operator()() const {
         return internal_use_do_not_use::call(receiver.value, flags, data.data(), data.size());
      }
   };

} // namespace eosio
