#pragma once

#include <cstdint>
#include <string>
#include <cstring>
#include <string_view>
#include <vector>
#include <optional>
#include <charconv>

inline eosio::chain::version_t extract_version_from_json_abi(std::vector<char> abi) {
   std::string_view sv(abi.data(), abi.size());

   constexpr const char* pattern = R"("version": "eosio::abi/)";
   if (auto pos = sv.find(pattern); pos != std::string_view::npos) {
      return eosio::chain::version_t{sv.substr(pos + strlen(pattern))};
   }
   return {};
}
