#pragma once
#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup instant_finality_c Instant_finality C API
 * @ingroup c_api
 * @brief Defines %C Instant_finality API
 */

/**
 * Submits a finalizer policy change to Instant Finality
 * 
 * @param data - pointer finalizer_set object packed as bytes
 * @param len  - size of packed finalazer_set object
 * @pre `data` is a valid pointer to a range of memory at least `len` bytes long that contains packed abi_finalizer_policy data
 * abi_finalizer_policy structure is defined in instant_finality.hpp
 */
__attribute__((eosio_wasm_import))
void set_finalizers( const char* data, uint32_t len );

#ifdef __cplusplus
}
#endif
