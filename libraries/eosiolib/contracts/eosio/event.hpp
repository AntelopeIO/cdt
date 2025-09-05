/**
 *  @file
 */
#pragma once
#include <vector>
#include <variant>
#include "action.hpp"
#include "../../core/eosio/asset.hpp"
#include "../../core/eosio/crypto.hpp"
#include <bluegrass/meta/for_each.hpp>
#include <bluegrass/meta/refl.hpp>

namespace eosio {

// ---------------------------------------------------------------------------------------------------
namespace internal_use_do_not_use {
extern "C" {
__attribute__((eosio_wasm_import)) void emit_event(const void* tags, uint32_t tags_num_bytes, const void* data,
                                                   uint32_t data_num_bytes);
}
} // namespace internal_use_do_not_use

// ---------------------------------------------------------------------------------------------------
struct event_header {
   struct event_header_v0 {
      checksum256 checksum; // checksum of the canonical event string representation

      EOSLIB_SERIALIZE(event_header_v0, (checksum))
   };

   std::variant<event_header_v0> header;

   EOSLIB_SERIALIZE(event_header, (header))
};

#if 1
// --------------------------------------------------------------------------
// Specializations for these template functions (for each event defined)
// should be defined in the temporary file created in `codegen.hpp` during
// the contract compilation.
// `get_canonical_string()` should return a constant string generated at
// compile time.
// --------------------------------------------------------------------------
template <class Event, std::enable_if_t<std::is_class_v<Event>, bool> = true>
std::vector<uint64_t> get_event_tags(const Event& ev, void* checksum);

template <class Event, std::enable_if_t<std::is_class_v<Event>, bool> = true>
constexpr std::string get_canonical_string(const Event& ev);

template <class Event, std::enable_if_t<std::is_class_v<Event>, bool> = true>
checksum256 get_event_checksum(const Event& ev);

#else
// -----------------------------------------------------------------------------
// Another way of doing the above, assuming that traits for the attributes and
// member names are generated during the contract compilation in  `codegen.hpp`.
// However this would not allow the `canonical_string` to be known at
// compile time, so we micht as well do the above.
// -----------------------------------------------------------------------------

// ----------------------------------- test ----------------------------------------------------------
// llvm/clang inserts
template <class T> struct eosio_traits {};

struct [[eosio::event("transfer")]] transfer_event {
   [[eosio::indexed, eosio::notify]] name        from;
   [[eosio::indexed, eosio::notify]] name        to;
                                     asset       quantity;
   [[eosio::indexed]]                std::string memo;
};

template <> struct eosio_traits<transfer_event> {
   static constexpr std::array<bool, 4> indexed{true, true, false, true};
   static constexpr std::array<bool, 4> notify{true, true, false, false};
   static const     std::array<std::string, 4> names{"from", "to", "quantity", "memo"};
};
// ----------------------------------- end test ----------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
template <class Event, std::enable_if_t<std::is_class_v<Event>, bool> = true>
std::vector<uint64_t> get_event_tags(const Event& ev) {}

// ---------------------------------------------------------------------------------------------------
template <class Event, std::enable_if_t<std::is_class_v<Event>, bool> = true>
std::string get_canonical_string(const Event& ev) {
   std::string s;
   s.reserve(256);

   auto names = boost::pfr::names_as_array<Event>(); // requires c++20
   // https://github.com/boostorg/pfr/blob/5034bf55fb6bd1efa7a14955bbb7171c2b3491fc/include/boost/pfr/detail/core_name14_disabled.hpp#L45

   bluegrass::meta::for_each_field(ev, [&](const auto& field, auto i) {
      constexpr auto idx = decltype(i)::value;
      constexpr bool indexed = eosio_traits<Event>::indexed[idx];
      constexpr bool notify  = eosio_traits<Event>::notify[idx];
      if constexpr (indexed) {
         if constexpr (notify)
            s += "[[eosio::indexed, eosio::notify]]";
         else
            s += "[[eosio::indexed]]";
      } else if constexpr (notify)
         s += "[[eosio::notify]]";
      s += bluegrass::meta::type_name<std::decay_t<decltype(field)>>();
      s += " ";
      s += eosio_traits<Event>::names[idx];
      s += ";";
   });

   return s;
}

// ---------------------------------------------------------------------------------------------------
template <class Event>
checksum256 get_event_checksum(const Event& ev) {
   auto s = get_canonical_string(ev);
   return sha256(s.data(), static_cast<uint32_t>(s.size()));
}
#endif


// ---------------------------------------------------------------------------------------------------
// `Event` must implement:
//   constexpr checksum256 get_event_checksum(); // returns the canonical signature of the event definition
//   std::vector<uint64_t> get_event_tags();      // returns tag1..tagN for this event
// ---------------------------------------------------------------------------------------------------
template <class Event>
void emit_event(Event&& event) {
   checksum256 checksum = get_event_checksum<std::decay_t<Event>>(event);
   event_header ev_header{.header = event_header::event_header_v0{checksum}};

   const std::vector<char>     data{pack(std::forward_as_tuple(ev_header, event))};
   const std::vector<uint64_t> tags{get_event_tags<std::decay_t<Event>>(event, checksum.data())};

   internal_use_do_not_use::emit_event(tags.data(), static_cast<uint32_t>(tags.size() * sizeof(uint64_t)),
                                       data.data(), static_cast<uint32_t>(data.size()));
}

} // namespace eosio
