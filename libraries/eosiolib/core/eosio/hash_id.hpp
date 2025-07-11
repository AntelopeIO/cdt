#pragma once

#include "check.hpp"
#include "serialize.hpp"
#include "reflect.hpp"

#include <string_view>

namespace eosio {
   struct hash_id {
   public:
      static constexpr uint32_t max_length = 128;

      enum class raw : uint64_t {};

      constexpr explicit hash_id( hash_id::raw r )
      :id(static_cast<uint64_t>(r)) {}

      constexpr hash_id() : id(0) {}

      constexpr explicit hash_id( std::string_view s )
      : id(djbh_hash(s))
      {
         validate(s);
      }

      constexpr void validate(std::string_view str) {
         if (str.empty()) {
            eosio::check(false, "string cannot be empty to be an hash_id");
         }

         if (str.length() > max_length) {
            eosio::check(false, "string is too long be a valid hash_id. must be less than or equal to " + std::to_string(max_length));
         }

         // cannot use std::isalpha in constexpr function
         if (! ((str[0] >= 'A' && str[0] <= 'Z') || (str[0] >= 'a' && str[0] <= 'z') || str[0] == '_') ) {
            eosio::check(false, "string must start with a letter or _ to be a valid hash_id.");
         }

         for (char c : str.substr(1)) {
            // cannot use std::isalnum in constexpr function
            if (! ((str[0] >= 'A' && str[0] <= 'Z') || (str[0] >= 'a' && str[0] <= 'z') || (str[0] >= '0' && str[0] <= '9') || str[0] == '_') ) {
               eosio::check(false, "string contains a character " + std::string{c} + " that is not a letter, number, or _");
            }
         }
      }

      static constexpr uint64_t djbh_hash(std::string_view s) {
         uint64_t hash = 5381;
         for (char c : s) {
            hash = ((hash << 5) + hash) + static_cast<uint8_t>(c); // hash * 33 + c
         }
         return hash;
      }

      constexpr operator raw()const { return raw(id); }

      uint64_t id = 0;

      CDT_REFLECT(id);
      EOSLIB_SERIALIZE( hash_id, (id) )
   }; /// namespace hash_id
} /// namespace eosio

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
template <typename T, T... Str>
inline constexpr eosio::hash_id operator""_i() {
   constexpr auto x = eosio::hash_id{std::string_view{eosio::detail::to_const_char_arr<Str...>::value, sizeof...(Str)}};
   return x;
}
#pragma clang diagnostic pop
