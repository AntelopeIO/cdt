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

  std::string hex_string;
  const char *hex_digits = "0123456789abcdef";

  const int num_digits = (sizeof(WideInt) * 2);

  hex_string.resize(num_digits);
  char *hex_chars = &hex_string[0];
  int leading_zeros = 0;

  for (int i = num_digits - 1; i >= 0; --i) {
    int least_significant_digit = value & 0xF;
    if (least_significant_digit != 0)
      leading_zeros = 0;
    else
      leading_zeros++;
    hex_chars[i] = hex_digits[least_significant_digit];
    value >>= 4; // Shift right by 4 bits
  }

  return hex_string.substr(leading_zeros);
}

template <typename WideInt,
          class = typename std::enable_if_t<std::is_unsigned<WideInt>::value>>
WideInt from_hex(const std::string &hex_string) {
  eosio::check(!hex_string.empty(),
               "eosio::wideint::from_hex Empty hexadecimal string");

  const std::size_t start_idx =
      (hex_string.size() > 2 && hex_string.substr(0, 2) == "0x") ? 2 : 0;

  WideInt result = 0;

  for (std::size_t i = start_idx; i < hex_string.size(); ++i) {
    const char c = hex_string[i];

    if (c >= '0' && c <= '9')
      result = (result << 4) | (c - '0');
    else if (c >= 'a' && c <= 'f')
      result = (result << 4) | (c - 'a' + 10);
    else if (c >= 'A' && c <= 'F')
      result = (result << 4) | (c - 'A' + 10);
    else
      eosio::check(false,
                   "eosio::wideint::from_hex Invalid hexadecimal string");
  }

  return result;
}

} // namespace eosio::wideint
