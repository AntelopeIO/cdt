/**
 *  @file
 *  @copyright defined in eosio.cdt/LICENSE.txt
 */

#include <initializer_list>
#include <limits>
#include <vector>

#include <eosio/tester.hpp>
#include <eosio/datastream.hpp>
#include <eosio/varint.hpp>

using std::numeric_limits;

using eosio::datastream;
using eosio::unsigned_int;
using eosio::signed_int;

static constexpr uint32_t u32min = numeric_limits<uint32_t>::min(); // 0
static constexpr uint32_t u32max = numeric_limits<uint32_t>::max(); // 4294967295

static constexpr int32_t i32min = numeric_limits<int32_t>::min(); // -2147483648
static constexpr int32_t i32max = numeric_limits<int32_t>::max(); //  2147483647

// Defined in `eosio.cdt/libraries/eosio/varint.hpp`
EOSIO_TEST_BEGIN(unsigned_int_type_test)
   //// unsigned_int(uint32_t)
   CHECK_EQUAL( unsigned_int{}.value, 0 )
   CHECK_EQUAL( unsigned_int{u32min}.value, 0 )
   CHECK_EQUAL( unsigned_int{u32max}.value, 4294967295 )

   //// unsigned_int(T)
   CHECK_EQUAL( unsigned_int{uint8_t{0}}.value,  0 )
   CHECK_EQUAL( unsigned_int{uint16_t{1}}.value, 1 )
   CHECK_EQUAL( unsigned_int{uint32_t{2}}.value, 2 )
   CHECK_EQUAL( unsigned_int{uint64_t{3}}.value, 3 )

   // -----------------
   // operator T()const
   CHECK_EQUAL( unsigned_int{0}.operator bool(), false )
   CHECK_EQUAL( unsigned_int{1}.operator bool(), true  )

   // ---------------------------------
   // unsigned_int& operator=(uint32_t)
   static const unsigned_int ui0{42};
   unsigned_int ui1{};

   ui1 = ui0;
   CHECK_EQUAL( ui0 == ui1, true )

   // ------------------------------------------------------------
   // friend bool operator==(const unsigned_int&, const uint32_t&)
   CHECK_EQUAL( unsigned_int{42} == uint32_t{42}, true  )
   CHECK_EQUAL( unsigned_int{42} == uint32_t{43}, false )

   // ------------------------------------------------------------
   // friend bool operator==(const uint32_t&, const unsigned_int&)
   CHECK_EQUAL( uint32_t{42} == unsigned_int{42}, true  )
   CHECK_EQUAL( uint32_t{43} == unsigned_int{42}, false )

   // ----------------------------------------------------------------
   // friend bool operator==(const unsigned_int&, const unsigned_int&)
   CHECK_EQUAL( unsigned_int{42} == unsigned_int{42}, true  )
   CHECK_EQUAL( unsigned_int{42} == unsigned_int{43}, false )

   // ------------------------------------------------------------
   // friend bool operator!=(const unsigned_int&, const uint32_t&)
   CHECK_EQUAL( unsigned_int{42} != uint32_t{42}, false )
   CHECK_EQUAL( unsigned_int{42} != uint32_t{43}, true  )

   // ------------------------------------------------------------
   // friend bool operator!=(const uint32_t&, const unsigned_int&)
   CHECK_EQUAL( uint32_t{42} != unsigned_int{42}, false )
   CHECK_EQUAL( uint32_t{43} != unsigned_int{42}, true  )

   // ----------------------------------------------------------------
   // friend bool operator!=(const unsigned_int&, const unsigned_int&)
   CHECK_EQUAL( unsigned_int{42} != unsigned_int{42}, false )
   CHECK_EQUAL( unsigned_int{42} != unsigned_int{43}, true  )

   // ------------------------------------------------------------
   // friend bool operator< (const unsigned_int&, const uint32_t&)
   CHECK_EQUAL( unsigned_int{42} < uint32_t{42}, false )
   CHECK_EQUAL( unsigned_int{42} < uint32_t{43}, true  )

   // ------------------------------------------------------------
   // friend bool operator< (const uint32_t&, const unsigned_int&)
   CHECK_EQUAL( uint32_t{42} < unsigned_int{42}, false )
   CHECK_EQUAL( uint32_t{42} < unsigned_int{43}, true  )

   // ----------------------------------------------------------------
   // friend bool operator< (const unsigned_int&, const unsigned_int&)
   CHECK_EQUAL( unsigned_int{42} < unsigned_int{42}, false )
   CHECK_EQUAL( unsigned_int{42} < unsigned_int{43}, true  )

   // ------------------------------------------------------------
   // friend bool operator>=(const unsigned_int&, const uint32_t&)
   CHECK_EQUAL( unsigned_int{42} >= uint32_t{42}, true  )
   CHECK_EQUAL( unsigned_int{42} >= uint32_t{43}, false )

   // ------------------------------------------------------------
   // friend bool operator>=(const uint32_t&, const unsigned_int&)
   CHECK_EQUAL( uint32_t{42} >= unsigned_int{42}, true  )
   CHECK_EQUAL( uint32_t{42} >= unsigned_int{43}, false )

   // ----------------------------------------------------------------
   // friend bool operator>=(const unsigned_int&, const unsigned_int&)
   CHECK_EQUAL( unsigned_int{42} >= unsigned_int{42}, true  )
   CHECK_EQUAL( unsigned_int{42} >= unsigned_int{43}, false )

   // ---------------------------------------------------------------
   // friend DataStream& operator<<(DataStream&, const unsigned_int&)
   // friend DataStream& operator>>(DataStream&, unsigned_int&)
   static constexpr uint16_t buffer_size{256};
   char datastream_buffer[buffer_size]; // Buffer for the datastream to point to

   datastream<const char*> ds{datastream_buffer, buffer_size};

   static const unsigned_int cui{42};
   unsigned_int ui{};
   ds << cui;
   ds.seekp(0);
   ds >> ui;
   CHECK_EQUAL( cui, ui)
EOSIO_TEST_END

/// One `unsigned_int` decoded from the front of a byte string: its value and how many bytes it used.
struct decoded_unsigned_int {
   uint32_t value;
   size_t   bytes_read;
};

/// Decodes one `unsigned_int` from the front of `bytes`.
static decoded_unsigned_int decode_unsigned_int( std::initializer_list<uint8_t> bytes ) {
   const std::vector<char> buffer( bytes.begin(), bytes.end() );
   datastream<const char*> ds{ buffer.data(), buffer.size() };
   unsigned_int ui{};
   ds >> ui;
   return { ui.value, ds.tellp() };
}

// Defined in `eosio.cdt/libraries/eosio/varint.hpp`: decoding reads every byte string the way the node does
EOSIO_TEST_BEGIN(unsigned_int_decode_test)
   static constexpr size_t max_encoded_size = 5;

   // The shortest encoding at each width decodes to its value and is read to its end.
   struct width_case {
      uint32_t value;
      size_t   bytes;
   };
   for( const width_case& c : { width_case{ 0, 1 }, width_case{ 0x7f, 1 }, width_case{ 0x80, 2 },
                                width_case{ 0x3fff, 2 }, width_case{ 0x4000, 3 }, width_case{ 0x1fffff, 3 },
                                width_case{ 0x200000, 4 }, width_case{ 0xfffffff, 4 }, width_case{ 0x10000000, 5 },
                                width_case{ u32max, 5 } } ) {
      char buffer[max_encoded_size] = {};
      datastream<char*> out{ buffer, max_encoded_size };
      out << unsigned_int{ c.value };
      CHECK_EQUAL( out.tellp(), c.bytes )

      datastream<const char*> in{ buffer, c.bytes };
      unsigned_int ui{};
      in >> ui;
      CHECK_EQUAL( ui.value, c.value )
      CHECK_EQUAL( in.remaining(), size_t{ 0 } )
   }

   // A longer encoding of the same value, up to five bytes, decodes to that value and is read to its end.
   {
      const auto d = decode_unsigned_int( { 0x80, 0x00 } );
      CHECK_EQUAL( d.value, uint32_t{ 0 } )
      CHECK_EQUAL( d.bytes_read, size_t{ 2 } )
   }
   {
      const auto d = decode_unsigned_int( { 0xff, 0x80, 0x00 } );
      CHECK_EQUAL( d.value, uint32_t{ 0x7f } )
      CHECK_EQUAL( d.bytes_read, size_t{ 3 } )
   }
   {
      const auto d = decode_unsigned_int( { 0x80, 0x80, 0x80, 0x80, 0x00 } );
      CHECK_EQUAL( d.value, uint32_t{ 0 } )
      CHECK_EQUAL( d.bytes_read, size_t{ 5 } )
   }

   // A value ends after its fifth byte whatever that byte's continuation bit says; the next byte is not part of it.
   {
      const auto d = decode_unsigned_int( { 0x80, 0x80, 0x80, 0x80, 0x80, 0x05 } );
      CHECK_EQUAL( d.value, uint32_t{ 0 } )
      CHECK_EQUAL( d.bytes_read, size_t{ 5 } )
   }
   {
      const std::vector<char> buffer{ char( 0x81 ), char( 0x80 ), char( 0x80 ), char( 0x80 ), char( 0x81 ), 0x05 };
      datastream<const char*> ds{ buffer.data(), buffer.size() };
      unsigned_int first{}, second{};
      ds >> first >> second;
      CHECK_EQUAL( first.value, uint32_t{ 0x10000001 } )
      CHECK_EQUAL( second.value, uint32_t{ 5 } )
      CHECK_EQUAL( ds.remaining(), size_t{ 0 } )
   }

   // Bits of the fifth byte beyond the 32nd value bit are discarded.
   {
      const auto d = decode_unsigned_int( { 0xff, 0xff, 0xff, 0xff, 0x7f } );
      CHECK_EQUAL( d.value, u32max )
      CHECK_EQUAL( d.bytes_read, size_t{ 5 } )
   }
   {
      const auto d = decode_unsigned_int( { 0x80, 0x80, 0x80, 0x80, 0x70 } );
      CHECK_EQUAL( d.value, uint32_t{ 0 } )
      CHECK_EQUAL( d.bytes_read, size_t{ 5 } )
   }

   // An encoding cut short fails the stream's bounds check.
   CHECK_ASSERT( "get", ([]() { decode_unsigned_int( {} ); }) )
   CHECK_ASSERT( "get", ([]() { decode_unsigned_int( { 0x80 } ); }) )
   CHECK_ASSERT( "get", ([]() { decode_unsigned_int( { 0xff, 0xff, 0xff, 0xff } ); }) )
EOSIO_TEST_END

// Defined in `eosio.cdt/libraries/eosio/varint.hpp`
EOSIO_TEST_BEGIN(signed_int_type_test)
   //// signed_int(uint32_t)
   CHECK_EQUAL( signed_int{}.value, 0 )
   CHECK_EQUAL( signed_int{i32min}.value, -2147483648 )
   CHECK_EQUAL( signed_int{i32max}.value,  2147483647 )

   // -----------------------
   // operator int32_t()const
   CHECK_EQUAL( signed_int{}.operator int32_t(), 0 )
   CHECK_EQUAL( signed_int{i32min}.operator int32_t(), -2147483648 )
   CHECK_EQUAL( signed_int{i32max}.operator int32_t(),  2147483647 )

   // --------------------------------
   //  signed_int& operator=(const T&)
   static const int8_t i8{0};
   static const int16_t i16{1};
   static const int32_t i32{2};
   static const int64_t i64{3};

   signed_int si0{};
   signed_int si1{};
   signed_int si2{};
   signed_int si3{};

   si0 = i8;
   si1 = i16;
   si2 = i32;
   si3 = i64;

   CHECK_EQUAL( si0.value, 0 )
   CHECK_EQUAL( si1.value, 1 )
   CHECK_EQUAL( si2.value, 2 )
   CHECK_EQUAL( si3.value, 3 )

   // --------------------------
   // signed_int operator++(int)
   signed_int si_post_inc0{0};
   signed_int si_post_inc1{1};
   CHECK_EQUAL( si_post_inc0++.value, 0 )
   CHECK_EQUAL( si_post_inc1++.value, 1 )

   // ------------------------
   // signed_int& operator++()
   signed_int si_pre_inc0{0};
   signed_int si_pre_inc1{1};
   CHECK_EQUAL( ++si_pre_inc0.value, 1 )
   CHECK_EQUAL( ++si_pre_inc1.value, 2 )

   // ------------------------------------------------------------
   // friend bool operator==(const signed_int&, const uint32_t&)
   CHECK_EQUAL( signed_int{42} == int32_t{42}, true  )
   CHECK_EQUAL( signed_int{42} == int32_t{43}, false )

   // ----------------------------------------------------------
   // friend bool operator==(const uint32_t&, const signed_int&)
   CHECK_EQUAL( int32_t{42} == signed_int{42}, true  )
   CHECK_EQUAL( int32_t{43} == signed_int{42}, false )

   // ------------------------------------------------------------
   // friend bool operator==(const signed_int&, const signed_int&)
   CHECK_EQUAL( signed_int{42} == signed_int{42}, true  )
   CHECK_EQUAL( signed_int{42} == signed_int{43}, false )

   // ----------------------------------------------------------
   // friend bool operator!=(const signed_int&, const uint32_t&)
   CHECK_EQUAL( signed_int{42} != int32_t{42}, false )
   CHECK_EQUAL( signed_int{42} != int32_t{43}, true  )

   // ----------------------------------------------------------
   // friend bool operator!=(const uint32_t&, const signed_int&)
   CHECK_EQUAL( int32_t{42} != signed_int{42}, false )
   CHECK_EQUAL( int32_t{43} != signed_int{42}, true  )

   // ------------------------------------------------------------
   // friend bool operator!=(const signed_int&, const signed_int&)
   CHECK_EQUAL( signed_int{42} != signed_int{42}, false )
   CHECK_EQUAL( signed_int{42} != signed_int{43}, true  )

   // ----------------------------------------------------------
   // friend bool operator< (const signed_int&, const uint32_t&)
   CHECK_EQUAL( signed_int{42} < int32_t{42}, false )
   CHECK_EQUAL( signed_int{42} < int32_t{43}, true  )

   // ----------------------------------------------------------
   // friend bool operator< (const uint32_t&, const signed_int&)
   CHECK_EQUAL( int32_t{42} < signed_int{42}, false )
   CHECK_EQUAL( int32_t{42} < signed_int{43}, true  )

   // ------------------------------------------------------------
   // friend bool operator< (const signed_int&, const signed_int&)
   CHECK_EQUAL( signed_int{42} < signed_int{42}, false )
   CHECK_EQUAL( signed_int{42} < signed_int{43}, true  )

   // ----------------------------------------------------------
   // friend bool operator>=(const signed_int&, const uint32_t&)
   CHECK_EQUAL( signed_int{42} >= int32_t{42}, true  )
   CHECK_EQUAL( signed_int{42} >= int32_t{43}, false )

   // ----------------------------------------------------------
   // friend bool operator>=(const uint32_t&, const signed_int&)
   CHECK_EQUAL( int32_t{42} >= signed_int{42}, true  )
   CHECK_EQUAL( int32_t{42} >= signed_int{43}, false )

   // ------------------------------------------------------------
   // friend bool operator>=(const signed_int&, const signed_int&)
   CHECK_EQUAL( signed_int{42} >= signed_int{42}, true  )
   CHECK_EQUAL( signed_int{42} >= signed_int{43}, false )

   // ----------------------------------------------------------------
   // friend DataStream& operator<<(DataStream& ds, const signed_int&)
   // friend DataStream& operator>>(DataStream&, signed_int&)
   static constexpr uint16_t buffer_size{256};
   char datastream_buffer[buffer_size]; // Buffer for the datastream to point to

   datastream<const char*> ds{datastream_buffer, buffer_size};

   static const signed_int csi{-42};
   signed_int a{44}, b{(1<<30)+2}, c{-35}, d{-(1<<30)-2}; // Small+, Small-, Large+, Large-
   signed_int aa, bb, cc, dd;
   ds << a << b << c << d;
   ds.seekp(0);
   ds >> aa >> bb >> cc >> dd;
   CHECK_EQUAL( a, aa )
   CHECK_EQUAL( b, bb )
   CHECK_EQUAL( c, cc )
   CHECK_EQUAL( d, dd )
EOSIO_TEST_END

EOSIO_TEST_BEGIN(unsigned_int_constexpr_test)
   //// unsigned_int(uint32_t)
   static_assert( unsigned_int{}.value == 0 );
   static_assert( unsigned_int{u32min}.value == 0 );
   static_assert( unsigned_int{u32max}.value == 4294967295);

   //// unsigned_int(T)
   static_assert( unsigned_int{uint8_t{0}}.value ==  0 );
   static_assert( unsigned_int{uint16_t{1}}.value == 1 );
   static_assert( unsigned_int{uint32_t{2}}.value == 2 );
   static_assert( unsigned_int{uint64_t{3}}.value == 3 );

   // -----------------
   // operator T()const
   static_assert( unsigned_int{0}.operator bool() == false );
   static_assert( unsigned_int{1}.operator bool() == true );

   // ---------------------------------
   // unsigned_int& operator=(uint32_t)
   constexpr unsigned_int ui0{42};
   constexpr unsigned_int ui1 = ui0;
   static_assert( ui0 == ui1 );

   // ------------------------------------------------------------
   // friend bool operator==(const unsigned_int&, const uint32_t&)
   static_assert( unsigned_int{42} == uint32_t{42} );
   static_assert( (unsigned_int{42} == uint32_t{43}) == false );

   // ------------------------------------------------------------
   // friend bool operator==(const uint32_t&, const unsigned_int&)
   static_assert( uint32_t{42} == unsigned_int{42} );
   static_assert( (uint32_t{43} == unsigned_int{42}) == false );

   // ----------------------------------------------------------------
   // friend bool operator==(const unsigned_int&, const unsigned_int&)
   static_assert( unsigned_int{42} == unsigned_int{42} );
   static_assert( (unsigned_int{42} == unsigned_int{43}) == false);

   // ------------------------------------------------------------
   // friend bool operator!=(const unsigned_int&, const uint32_t&)
   static_assert( (unsigned_int{42} != uint32_t{42}) == false );
   static_assert( unsigned_int{42} != uint32_t{43} );

   // ------------------------------------------------------------
   // friend bool operator!=(const uint32_t&, const unsigned_int&)
   static_assert( (uint32_t{42} != unsigned_int{42}) == false );
   static_assert( uint32_t{43} != unsigned_int{42} );

   // ----------------------------------------------------------------
   // friend bool operator!=(const unsigned_int&, const unsigned_int&)
   static_assert( (unsigned_int{42} != unsigned_int{42}) == false );
   static_assert( unsigned_int{42} != unsigned_int{43} );

   // ------------------------------------------------------------
   // friend bool operator< (const unsigned_int&, const uint32_t&)
   static_assert( (unsigned_int{42} < uint32_t{42}) == false );
   static_assert( unsigned_int{42} < uint32_t{43} );

   // ------------------------------------------------------------
   // friend bool operator< (const uint32_t&, const unsigned_int&)
   static_assert( (uint32_t{42} < unsigned_int{42}) == false );
   static_assert( uint32_t{42} < unsigned_int{43} );

   // ----------------------------------------------------------------
   // friend bool operator< (const unsigned_int&, const unsigned_int&)
   static_assert( (unsigned_int{42} < unsigned_int{42}) == false );
   static_assert( unsigned_int{42} < unsigned_int{43} );

   // ------------------------------------------------------------
   // friend bool operator>=(const unsigned_int&, const uint32_t&)
   static_assert( unsigned_int{42} >= uint32_t{42} );
   static_assert( (unsigned_int{42} >= uint32_t{43}) == false );

   // ------------------------------------------------------------
   // friend bool operator>=(const uint32_t&, const unsigned_int&)
   static_assert( uint32_t{42} >= unsigned_int{42} );
   static_assert( (uint32_t{42} >= unsigned_int{43}) == false  );

   // ----------------------------------------------------------------
   // friend bool operator>=(const unsigned_int&, const unsigned_int&)
   static_assert( unsigned_int{42} >= unsigned_int{42} );
   static_assert( (unsigned_int{42} >= unsigned_int{43}) == false  );

   // ----------
   // misc tests
   static_assert( unsigned_int{0xff}.value == 255 );     // 1 byte
   static_assert( unsigned_int{0xffff}.value == 65535 ); // 2 bytes
   static_assert( unsigned_int{0xffffff}.value == 16777215 ); // 3 bytes
   static_assert( unsigned_int{0xffffffff}.value == 4294967295 ); // 4 bytes
   static_assert( unsigned_int{100l}.value == 100 );  // cast from long
   static_assert( sizeof(unsigned_int{0xf}) == 4 );
EOSIO_TEST_END

EOSIO_TEST_BEGIN(signed_int_constexpr_test)
   //// signed_int(uint32_t)
   static_assert( signed_int{}.value == 0 );
   static_assert( signed_int{i32min}.value == -2147483648 );
   static_assert( signed_int{i32max}.value ==  2147483647 );

   // -----------------------
   // operator int32_t()const
   static_assert( signed_int{}.operator int32_t() == 0 );
   static_assert( signed_int{i32min}.operator int32_t() == -2147483648 );
   static_assert( signed_int{i32max}.operator int32_t() ==  2147483647 );

   // --------------------------------
   //  signed_int& operator=(const T&)
   constexpr int8_t i8{0};
   constexpr int16_t i16{1};
   constexpr int32_t i32{2};
   constexpr int64_t i64{3};

   constexpr signed_int si0 = i8;
   constexpr signed_int si1 = i16;
   constexpr signed_int si2 = i32;
   constexpr signed_int si3 = i64;

   static_assert( si0.value == 0 );
   static_assert( si1.value == 1 );
   static_assert( si2.value == 2 );
   static_assert( si3.value == 3 );

   // --------------------------
   // signed_int operator++(int)
   constexpr signed_int si_post_inc0{ signed_int{0}++ };
   constexpr signed_int si_post_inc1{ signed_int{1}++ };
   static_assert( si_post_inc0.value == 0 );
   static_assert( si_post_inc1.value == 1 );

   // ------------------------
   // signed_int& operator++()
   constexpr signed_int si_pre_inc0{ ++signed_int{0} };
   constexpr signed_int si_pre_inc1{ ++signed_int{1} };
   static_assert( si_pre_inc0.value == 1 );
   static_assert( si_pre_inc1.value == 2 );

   // ------------------------------------------------------------
   // friend bool operator==(const signed_int&, const uint32_t&)
   static_assert( signed_int{42} == int32_t{42} );
   static_assert( (signed_int{42} == int32_t{43}) == false );

   // ----------------------------------------------------------
   // friend bool operator==(const uint32_t&, const signed_int&)
   static_assert( int32_t{42} == signed_int{42} );
   static_assert( (int32_t{43} == signed_int{42}) == false );

   // ------------------------------------------------------------
   // friend bool operator==(const signed_int&, const signed_int&)
   static_assert( signed_int{42} == signed_int{42} );
   static_assert( (signed_int{42} == signed_int{43}) == false );

   // ----------------------------------------------------------
   // friend bool operator!=(const signed_int&, const uint32_t&)
   static_assert( (signed_int{42} != int32_t{42}) == false );
   static_assert( signed_int{42} != int32_t{43} );

   // ----------------------------------------------------------
   // friend bool operator!=(const uint32_t&, const signed_int&)
   static_assert( (int32_t{42} != signed_int{42}) == false );
   static_assert( int32_t{43} != signed_int{42} );

   // ------------------------------------------------------------
   // friend bool operator!=(const signed_int&, const signed_int&)
   static_assert( (signed_int{42} != signed_int{42}) == false );
   static_assert( signed_int{42} != signed_int{43} );

   // ----------------------------------------------------------
   // friend bool operator< (const signed_int&, const uint32_t&)
   static_assert( (signed_int{42} < int32_t{42}) == false );
   static_assert( signed_int{42} < int32_t{43} );

   // ----------------------------------------------------------
   // friend bool operator< (const uint32_t&, const signed_int&)
   static_assert( (int32_t{42} < signed_int{42}) == false );
   static_assert( int32_t{42} < signed_int{43} );

   // ------------------------------------------------------------
   // friend bool operator< (const signed_int&, const signed_int&)
   static_assert( (signed_int{42} < signed_int{42}) == false );
   static_assert( signed_int{42} < signed_int{43} );

   // ----------------------------------------------------------
   // friend bool operator>=(const signed_int&, const uint32_t&)
   static_assert( signed_int{42} >= int32_t{42} );
   static_assert( (signed_int{42} >= int32_t{43}) == false );

   // ----------------------------------------------------------
   // friend bool operator>=(const uint32_t&, const signed_int&)
   static_assert( int32_t{42} >= signed_int{42} );
   static_assert( (int32_t{42} >= signed_int{43}) == false );

   // ------------------------------------------------------------
   // friend bool operator>=(const signed_int&, const signed_int&)
   static_assert( signed_int{42} >= signed_int{42} );
   static_assert( (signed_int{42} >= signed_int{43}) == false );

   // ----------
   // misc tests
   static_assert( signed_int{0xff}.value == 255 );     // 1 byte
   static_assert( signed_int{0xffff}.value == 65535 ); // 2 bytes
   static_assert( signed_int{0xffffff}.value == 16777215 ); // 3 bytes
   static_assert( signed_int{0x7fffffff}.value == 2147483647 ); // 4 bytes
   static_assert( signed_int{-0x7fffffff}.value == -2147483647 ); // 4 bytes
   static_assert( signed_int{-0x80000000l}.value == -2147483648 ); // 4 bytes
   static_assert( signed_int{100u}.value == 100 ); // cast from unsigned
   static_assert( sizeof(signed_int{0xf}) == 4 );
EOSIO_TEST_END

int main(int argc, char* argv[]) {
   bool verbose = false;
   if( argc >= 2 && std::strcmp( argv[1], "-v" ) == 0 ) {
      verbose = true;
   }
   silence_output(!verbose);

   EOSIO_TEST(unsigned_int_type_test)
   EOSIO_TEST(unsigned_int_decode_test)
   EOSIO_TEST(signed_int_type_test);
   EOSIO_TEST(unsigned_int_constexpr_test);
   EOSIO_TEST(signed_int_constexpr_test);
   return has_failed();
}
