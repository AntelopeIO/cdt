#pragma once

#include "serialize.hpp"
#include "print.hpp"
#include "check.hpp"
#include "varint.hpp"

#include <vector>

namespace eosio {

// -------------------------------------------------------------------------------
//      see https://github.com/AntelopeIO/spring/wiki/ABI-1.3:-bitset-type
// -------------------------------------------------------------------------------


// stores a bitset in a std::vector<uint8_t>
//
// - bits 0-7 in first byte, 8-15 in second, ...
// - least significant bit of byte 0 is bit 0 of bitset.
// - unused bits must be zero.
// ---------------------------------------------------------------------------------
struct bitset {
   using buffer_type                         = std::vector<uint8_t>;
   using size_type                           = uint32_t;
   static constexpr size_type bits_per_block = 8;
   static constexpr size_type npos           = static_cast<size_type>(-1);

   static constexpr size_type calc_num_blocks(size_type num_bits) {
      return (num_bits + bits_per_block - 1) / bits_per_block;
   }

   static size_type block_index(size_type pos) noexcept { return pos / bits_per_block; }
   static uint8_t   bit_index(size_type pos)   noexcept { return static_cast<uint8_t>(pos % bits_per_block); }
   static uint8_t   bit_mask(size_type pos)    noexcept { return uint8_t(1) << bit_index(pos); }

   size_type size() const { return m_num_bits; }

   size_type num_blocks() const { return m_bits.size(); }

   void resize(size_type num_bits) {
      m_bits.resize(calc_num_blocks(num_bits), 0);
      m_num_bits = num_bits;
      zero_unused_bits();
   }

   void set(size_type pos) {
      assert(pos < m_num_bits);
      m_bits[block_index(pos)] |= bit_mask(pos);
   }

   void clear(size_type pos) {
      assert(pos < m_num_bits);
      m_bits[block_index(pos)] &= ~bit_mask(pos);
   }

   bool operator[](size_type pos) const {
      assert(pos < m_num_bits);
      return !!(m_bits[block_index(pos)] & bit_mask(pos));
   }

   void zero_all_bits() {
      for (auto& byte : m_bits)
         byte = 0;
   }

   void zero_unused_bits() {
      assert (m_bits.size() == calc_num_blocks(m_num_bits));

      // if != 0 this is the number of bits used in the last block
      const size_type extra_bits = bit_index(size());

      if (extra_bits != 0)
         m_bits.back() &= (uint8_t(1) << extra_bits) - 1;
   }

   bool unused_bits_zeroed() const {
      // if != 0 this is the number of bits used in the last block
      const size_type extra_bits = bit_index(size());
      return extra_bits == 0 || (m_bits.back() & ~((uint8_t(1) << extra_bits) - 1)) == 0;
   }

   friend auto operator<(const bitset& a, const bitset& b) {
      return std::tuple(a.m_num_bits, a.m_bits) < std::tuple(b.m_num_bits, b.m_bits);
   }

   friend bool operator==(const bitset& a, const bitset& b) {
      return std::tuple(a.m_num_bits, a.m_bits) == std::tuple(b.m_num_bits, b.m_bits);
   }

   uint8_t& byte(size_t i) {
      assert(i < m_bits.size());
      return m_bits[i];
   }

   const uint8_t& byte(size_t i) const {
      assert(i < m_bits.size());
      return m_bits[i];
   }

   std::string to_string() const {
      std::string res;
      res.resize(size());
      size_t idx = 0;
      for (auto i = size(); i-- > 0;)
         res[idx++] = (*this)[i] ? '1' : '0';
      return res;
   }

   void print() const {
      auto s = to_string();
      if (!s.empty())
         printl(s.data(), s.size());
   }

   // binary representation
   // ---------------------
   // The bitset first encodes the number of bits it contains as a varint, then encodes
   // (size+8-1)/8 bytes into the stream. The first byte represents bits 0-7, the next 8-15,
   // and so on; i.e. LSB first.
   // Within a byte, the least significant bit stores the smaller bitset index.
   // Unused bits should be written as 0.
   //
   // This matches the storage scheme of bitset above
   // ---------------------------------------------------------------------------------------
   template <typename DataStream>
   friend DataStream& operator>>(DataStream& stream, bitset& obj) {
      unsigned_int num_bits(0);
      stream >> num_bits;
      obj.resize(num_bits.value);
      if (obj.size() > 0) {
         auto num_blocks = bitset::calc_num_blocks(obj.size());
         for (size_t i = 0; i < num_blocks; ++i)
            stream >> obj.byte(i);
         obj.zero_unused_bits();
         assert(obj.unused_bits_zeroed());
      }
      return stream;
   }

   template <typename DataStream>
   friend DataStream& operator<<(DataStream& stream, const bitset& obj) {
      unsigned_int num_bits(obj.size());
      stream << num_bits;
      if (obj.size() > 0) {
         auto num_blocks = bitset::calc_num_blocks(obj.size());
         assert(num_blocks >= 1);
         for (size_t i = 0; i < num_blocks; ++i)
            stream << obj.byte(i);
      }
      return stream;
   }

private:
   size_type   m_num_bits{0}; // members order matters for comparison operators
   buffer_type m_bits;        // must be after `m_num_bits`
};

constexpr const char* get_type_name(bitset*) { return "bitset"; }

} // namespace eosio
