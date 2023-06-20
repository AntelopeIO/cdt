/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
// #pragma once

// #if defined __llvm__
// typedef _BitInt(128) int128_t;
// typedef unsigned _BitInt(128) uint128_t;
// typedef _BitInt(256) int256_t;
// typedef unsigned _BitInt(256) uint256_t;
// #elif defined __GNUC__
// typedef __int128 int128_t;
// typedef unsigned __int128 uint128_t;
// #endif



#pragma once

#if defined __GNUC__
typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;
#endif

