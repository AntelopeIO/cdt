/*
   base64 encoding and decoding with C++.
   More information at
     https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
     https://github.com/ReneNyffenegger/cpp-base64

   Version: 2.rc.09 (release candidate)

   Copyright (C) 2004-2017, 2020-2022 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

#include "check.hpp"

#include <algorithm>
#include <string_view>

namespace eosio {

namespace detail {
std::string base64_encode(unsigned char const*, size_t len, bool url = false);
std::string base64_decode(std::string_view s, bool remove_linebreaks = false);

} // detail namespace

inline std::string base64_encode(std::string_view enc) {
   return detail::base64_encode(reinterpret_cast<const unsigned char*>(enc.data()), enc.size(), false);
}
inline std::string base64_decode(std::string_view encoded_string) {
   return detail::base64_decode(encoded_string, false);
}
inline std::string base64url_encode(std::string_view enc) {
   return detail::base64_encode(reinterpret_cast<const unsigned char*>(enc.data()), enc.size(), true);
}
inline std::string base64url_decode(std::string_view encoded_string) {
   return detail::base64_decode(encoded_string, true);
}

} // namespace eosio
