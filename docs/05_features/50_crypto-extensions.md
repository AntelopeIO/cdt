---
content_title: Crypto Extensions
---

As of `v3.0` crypto host functions were extended to include
- `mod_exp`: Big integer modular exponentiation
- `alt_bn128_add`, `alt_bn128_mul`, `alt_bn128_pair`: Add, multiply, and pairing check functions for the `alt_bn128` elliptic curve
- `blake2_f`: `BLAKE2b F` compression function
- `sha3`: sha3` hash function using `SHA3 NIST`
- `keccak`: `Keccak256` hash function using `SHA3 Keccak`
- `k1_recover`: Safe `ECDSA` uncompressed pubkey recover for the `secp256k1` curve

In `v3.0`, `C` format was supported; in `v3.1` and newer versions, `C++` format was added for better data abstraction.

## Prerequisites

- In `nodeos`, activate protocol feature `CRYPTO_PRIMITIVES` (`68d6405cb8df3de95bd834ebb408196578500a9f818ff62ccc68f60b932f7d82`)
- In smart contract code, include `crypto_ext.hpp`

## C Format

- `int32_t alt_bn128_add( const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* result, uint32_t result_len )`
Perform addition operation on the elliptic curve `alt_bn128`, store the result in `result`, and return `0` if success otherwise `-1`
- `int32_t alt_bn128_mul( const char* g1, uint32_t g1_len, const char* scalar, uint32_t scalar_len, char* result, uint32_t result_len )`
Perform scalar multiplication operation on the elliptic curve `alt_bn128`
- `int32_t alt_bn128_pair( const char* pairs, uint32_t pairs_len )`
Perform Optimal-Ate pairing check elliptic curve `alt_bn128`, and return `0` if `true` and successful, `1` if `false`,  otherwise `-1`
- `int32_t mod_exp( const char* base, uint32_t base_len, const char* exp, uint32_t exp_len, const char* mod, uint32_t mod_len, char* result, uint32_t result_len )`
Calculate `( BASE^EXP ) % MOD`, store in `result`, and return `0` if successful, otherwise `-1`
- `int32_t blake2_f( uint32_t rounds, const char* state, uint32_t state_len, const char* msg, uint32_t msg_len, const char* t0_offset, uint32_t t0_len, const char* t1_offset, uint32_t t1_len, int32_t final, char* result, uint32_t result_len)`
Implement BLAKE2 compression function `F`. Return `0` if success otherwise `-1`
- `eosio::checksum256 sha3(const char* data, uint32_t length)`
Return hash of `data` using `SHA3 NIST`
- `void assert_sha3(const char* data, uint32_t length, const eosio::checksum256& hash)`
Test if the SHA3 hash generated from data matches the provided digest
- `eosio::checksum256 keccak(const char* data, uint32_t length)`
Return hash of `data` using `SHA3 Keccak`
- `void assert_keccak(const char* data, uint32_t length, const eosio::checksum256& hash)
Test if the SHA3 keccak hash generated from data matches the provided digest
- `int32_t k1_recover( const char* sig, uint32_t sig_len, const char* dig, uint32_t dig_len, char* pub, uint32_t pub_len )`
Calculates the uncompressed public key used for a given signature on a given digest. Return `0` if success otherwise `-1`

## C++ Format

C++ types were added to represent `G1` and `G2` points (read and write) and views (read-only), and represent big integers. Their definitions are

### Types

```c++
   /**
    * Abstracts mutable G1 and G2 points
    *
    */
   template <std::size_t Size = 32>
   struct ec_point {
      /**
       * Bytes of the x coordinate
       */
      std::vector<char> x;

      /**
       * Bytes of the y coordinate
       */
      std::vector<char> y;

      /**
       * Construct a point given x and y
       *
       * @param x_ - The x coordinate, a vector of chars
       * @param y_ - The y coordinate, a vector of chars
      */
      ec_point(std::vector<char>& x_, std::vector<char>& y_);

      /**
       * Construct a point given a serialized point
       *
       * @param p - The serialized point
       */
      ec_point(std::vector<char>& p);

      /**
       *  Return serialized point containing only x and y
       */
      std::vector<char> serialized() const;
   };

   /**
    * Abstracts read-only G1 and G2 points
    */
   template <std::size_t Size = 32>
   struct ec_point_view {
      /**
       * Pointer to the x coordinate
       */
      const char* x;

      /**
       * Pointer to the y coordinate
       */
      const char* y;

      /**
       * Number of bytes in each of x and y
       */
      uint32_t size;

      /**
       * Construct a point view from x and y
       *
       * @param x_     - The x coordinate, poiter to chars
       * @param x_size - x's size
       * @param y_     - The y coordinate, poiter to chars
       * @param y_size - y's size
       */
      ec_point_view(const char* x_, uint32_t x_size, const char* y_, uint32_t y_size);

      /**
       * Construct a point view from a serialized point
       *
       * @param p - The serialized point
       */
      ec_point_view(const std::vector<char>& p);

      /**
       * Construct a point view from a point
       *
       * @param p - The point
       */
      ec_point_view(const ec_point<Size>& p);

      /**
       *  Return serialized point containing only x and y
       */
      std::vector<char> serialized() const;
   };

   static constexpr size_t g1_coordinate_size = 32;
   static constexpr size_t g2_coordinate_size = 64;

   using g1_point = ec_point<g1_coordinate_size>;
   using g2_point = ec_point<g2_coordinate_size>;
   using g1_point_view = ec_point_view<g1_coordinate_size>;
   using g2_point_view = ec_point_view<g2_coordinate_size>;

   /**
    * Big integer.
    *
    *  @ingroup crypto
    */
   using bigint = std::vector<char>;
```

### Methods

- `alt_bn128_add`
```c++
    template <typename T>
    g1_point alt_bn128_add( const T& op1, const T& op2 )
```
Take two G1 points or G1 views as input and return a G1 point.
- `alt_bn128_mul`
```c++
   template <typename T>
   g1_point alt_bn128_mul( const T& g1, const bigint& scalar)
```
Take a G1 point or view and a bigint as input and return a G1 point
- `alt_bn128_pair`
```c++
   template <typename G1_T, typename G2_T>
   int32_t alt_bn128_pair( const std::vector<std::pair<G1_T, G2_T>>& pairs )
```
Take a pair of G1 and G2 as input.
- `mod_exp`
```c++
   int32_t mod_exp( const bigint& base, const bigint& exp, const bigint& mod, bigint& result )
```
Take bigints as input

### Examples

- `alt_bn128_add`
```c++
   std::vector<char> x1, y1, x2, y2;

   // point
   eosio::g1_point point1 {x1, y1};
   eosio::g1_point point2 {x2, y2};
   auto result = eosio::alt_bn128_add(point1, point2);

   // view
   eosio::g1_point_view point_view1 {x1.data(), x1.size(), y1.data(), y1.size()};
   eosio::g1_point_view point_view2 {x2.data(), x2.size(), y2.data(), y2.size()};
   result = eosio::alt_bn128_add(point_view1, point_view2);
```

- `alt_bn128_mul`
```c++
   std::vector<char> x, y, scaler;
   eosio::bigint s {scalar};

   // point
   eosio::g1_point g1_point {x, y};
   auto result = eosio::alt_bn128_mul(g1_point, s);

   // view
   eosio::g1_point_view g1_view {x.data(), x.size(), y.data(), y.size()};
   result = eosio::alt_bn128_mul(g1_view, s);
```

- `alt_bn128_pair`
```c++
   std::vector<char> g1_a_x, g1_a_y, g2_a_x, g2_a_y, g1_b_x, g1_b_y, g2_b_x, g2_b_y;


   // point
   eosio::g1_point g1_a {g1_a_x, g1_a_y};
   eosio::g2_point g2_a {g2_a_x, g2_a_y};
   eosio::g1_point g1_b {g1_b_x, g1_b_y};
   eosio::g2_point g2_b {g2_b_x, g2_b_y};
   std::vector<std::pair<eosio::g1_point, eosio::g2_point>> pairs { {g1_a, g2_a}, {g1_b, g2_b} };
   auto result = eosio::alt_bn128_pair(pairs);

   // view
   eosio::g1_point_view g1_view_a {g1_a_x.data(), g1_a_x.size(), g1_a_y.data(), g1_a_y.size()};
   eosio::g2_point_view g2_view_a {g2_a_x.data(), g2_a_x.size(), g2_a_y.data(), g2_a_y.size()};
   eosio::g1_point_view g1_view_b {g1_b_x.data(), g1_b_x.size(), g1_b_y.data(), g1_b_y.size()};
   eosio::g2_point_view g2_view_b {g2_b_x.data(), g2_b_x.size(), g2_b_y.data(), g2_b_y.size()};
   std::vector<std::pair<eosio::g1_point_view, eosio::g2_point_view>> view_pairs { {g1_a, g2_a}, {g1_b, g2_b} };
   result = eosio::alt_bn128_pair(view_pairs);
```

- `mod_exp`
```c++
   std::vector<char> base, exp, modulo;
   eosio::bigint base_val {base};
   eosio::bigint exp_val {exp};
   eosio::bigint modulo_val {modulo};
   eosio::bigint result( modulo.size(), '\0' );

   auto rc = eosio::mod_exp(base_val, exp_val, modulo_val, result);
```
