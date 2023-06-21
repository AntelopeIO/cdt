#pragma once

#include <string>

#include "check.hpp"
#include "types.hpp"

namespace eosio::wideint {

template <typename WideInt,
          class = typename std::enable_if_t<std::is_unsigned<WideInt>::value>>
std::string to_hex(WideInt value) {
  if (value == 0)
    return "0";

  constexpr int num_digits = (sizeof(WideInt) * 2);
  constexpr int hex_digits_capacity = num_digits + 1; // +1 for null terminator

  char hex_digits[hex_digits_capacity];
  int digit_index = num_digits;
  int leading_zeros = 0;

  while (value != 0 && digit_index > 0) {
    int least_significant_digit = value & 0xF;
    if (least_significant_digit != 0)
      leading_zeros = 0;
    else
      leading_zeros++;
    hex_digits[--digit_index] = "0123456789abcdef"[least_significant_digit];
    value >>= 4;
  }

  return std::string(hex_digits + digit_index,
                     num_digits - digit_index - leading_zeros);
}

template <typename WideInt,
          class = typename std::enable_if_t<std::is_unsigned<WideInt>::value>>
WideInt from_hex(const std::string &hex_string) {
  eosio::check(!hex_string.empty(),
               "eosio::wideint::from_hex Empty hexadecimal string");

  const std::size_t start_idx =
      (hex_string.size() > 2 && hex_string[0] == '0' &&
       (hex_string[1] == 'x' || hex_string[1] == 'X'))
          ? 2
          : 0;

  WideInt result = 0;

  for (std::size_t i = start_idx; i < hex_string.size(); ++i) {
    const char c = hex_string[i];

    int val = 0;
    if (c <= '9') {
      if (c < '0')
        eosio::check(false, "eosio::wideint::from_hex Invalid hexadecimal string");

      val = c - '0';
    } else {
      if (c <= 'F') {
        if (c < 'A')
          eosio::check(false, "eosio::wideint::from_hex Invalid hexadecimal string");

        val = c - 'A' + 10;
      } else {
        if (c < 'a' || c > 'f')
          eosio::check(false, "eosio::wideint::from_hex Invalid hexadecimal string");

        val = c - 'a' + 10;
      }
    }

    result = (result << 4) | static_cast<WideInt>(val);
  }
  return result;
}

} // namespace eosio::wideint
