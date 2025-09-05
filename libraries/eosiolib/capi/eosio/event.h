#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup call_c Call C API
 * @ingroup c_api
 * @brief Logs an event
 *        First tag is tag1, the event identifier. tag0 is inserted by the host function.
 *        Each tag is a 64 byte value
 */
void emit_event(void* tags, uint32_t tags_num_bytes, void* data, uint32_t data_num_bytes);


#ifdef __cplusplus
}
#endif