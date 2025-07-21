#pragma once

#include "../../core/eosio/name.hpp"
#include "../../core/eosio/datastream.hpp"


/**
 * @defgroup contract Contract
 * @ingroup contracts
 * @ingroup types
 * @brief Defines contract type which is %base class for every EOSIO contract
 */

/**
 * Helper macros to reduce the verbosity for common contracts
 * @ingroup contract
 */
#define CONTRACT class [[eosio::contract]]
#define ACTION   [[eosio::action]] void
#define TABLE struct [[eosio::table]]

namespace eosio {

/**
 * %Base class for EOSIO contract.
 *
 * @ingroup contract
 * @details %A new contract should derive from this class, so it can make use of EOSIO_ABI macro.
 */
class contract {
   public:
      enum class exec_type_t : uint8_t {
         action,
         call
      };

      /**
       * Construct a new contract given the contract name
       *
       * @param self - The name of the account this contract is deployed on
       * @param first_receiver - The account the incoming action was first received at.
       * @param ds - The datastream used
       */
      contract( name self, name first_receiver, datastream<const char*> ds ):_self(self),_first_receiver(first_receiver),_ds(ds) {}

      /**
       *
       * Get this contract name
       *
       * @return name - The name of this contract
       */
      inline name get_self()const { return _self; }

      /**
       * The first_receiver name of the action this contract is processing.
       *
       * @return name - The first_receiver name of the action this contract is processing.
       */
      [[deprecated]]
      inline name get_code()const { return _first_receiver; }

      /**
       * The account the incoming action was first received at.
       *
       * @return name - The first_receiver name of the action this contract is processing.
       */
      inline name get_first_receiver()const { return _first_receiver; }

      /**
       * Get the datastream for this contract
       *
       * @return datastream<const char*> - The datastream for this contract
       */
      inline datastream<const char*>& get_datastream() { return _ds; }

      /**
       * Get the datastream for this contract
       *
       * @return datastream<const char*> - The datastream for this contract
       */
      inline const datastream<const char*>& get_datastream()const { return _ds; }

      /**
       * Whether this contract is for a sync call
       *
       * @return bool - Whether this contract is for a sync call
       */
      inline bool is_sync_call()const {
         check(_exec_type.has_value(), "too early to call is_sync_call(). _exec_type has not been set yet");
         return (*_exec_type == exec_type_t::call);
      }

      /**
       * Set the exectution type.
       *
       * @param type - The exectution type to be set.
       */
      inline void set_exec_type(exec_type_t type) {
         _exec_type = type;
      }

   protected:

      /**
       * The name of the account this contract is deployed on.
       */
      name _self;

      /**
       * The account the incoming action was first received at.
       */
      name _first_receiver;

      /**
       * The datastream for this contract
       */
      datastream<const char*> _ds = datastream<const char*>(nullptr, 0);

      /**
       * The execution type: action or sync call
       */
      std::optional<exec_type_t> _exec_type = std::nullopt; // use std::optional to prevent from being used before having value like in constructors
};
}
