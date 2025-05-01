#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup call_c Call C API
 * @ingroup c_api
 * @brief Defines API for querying call object and making a sync call
 */

/**
 *  Make a sync call in the context of this call's parent action or parent call
 *
 *  @param receiver - the name of the account that the sync call is made to
 *  @param flags - flags (bits) representing blockchain level requirements about
 *                 the call. Currently LSB bit indicates read-only. All other bits
 *                 are reserved to be 0
 *  @param data - the data of the sync call, which may include function name, arguments, and other information
 *  @return -1 if the receiver contract does not have sync call entry point or the entry point's signature is invalid, otherwise
 *  @return the number of bytes of the return value of the call. If the function is `void`, rturn `0`
 *
 */
__attribute__((eosio_wasm_import))
int64_t call(capi_name receiver, uint64_t flags, const char* data, size_t data_size);

/**
 * Copy up to `len` bytes of the return value of the most recent call to `mem`,
 * in the context of this call's parent action or parent call
 *
 *  @brief Copy the return value of the most recent call to the specified location
 *  @param mem - a pointer where up to `len` bytes of the return value will be copied
 *  @param len - length of the return value to be copied
 *  @return the number of bytes of the return value that can be retrieved (the number of total bytes of return value)
 */
__attribute__((eosio_wasm_import))
uint32_t get_call_return_value( void* mem, uint32_t len );

/**
 *  Copy up to `len` bytes of the current call data to `mem`
 *
 *  @brief Copy current call data to the specified location
 *  @param mem - a pointer where up to `len` bytes of the current call data will be copied
 *  @param len - length of the current call data to be copied
 *  @return the number of bytes of the data that can be retrieved (the number of total bytes of the data)
 */
__attribute__((eosio_wasm_import))
uint32_t get_call_data( void* mem, uint32_t len );

/**
 * Set the return value from `mem` with `len` bytes. This is to be retrieved by parent action
 * or parent call using `get_call_return_value`
 *
 *  @brief Copy the return value from the specified location
 *  @param mem - a pointer where `len` bytes of the return value will be copied from
 */
__attribute__((eosio_wasm_import))
void set_call_return_value( void* mem, uint32_t len );

#ifdef __cplusplus
}
#endif
/// @} call
