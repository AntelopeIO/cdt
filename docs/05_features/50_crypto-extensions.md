---
content_title: Crypto Extensions API
---


Antelope blockchain implements cryptographic functions for operations on points on elliptic curves, computing hashes 
of big integers, big integer modular exponentiation, and other operations useful for implementing 
cryptographic algorithms in your contracts.

In order to use the Crypto Extensions API you need to activate a protocol feature `CRYPTO_PRIMITIVES`
in `nodeos`. To do this you should call the following command in your command line:

`"cleos push action eosio activate ["6bcb40a24e49c26d0a60513b6aeb8551d264e4717f306b81a37a5afb3b47cedc"] -p eosio@active"`

And you need to include in the source code of your contract the following header: [crypto_ext.hpp](https://github.com/AntelopeIO/cdt/blob/main/libraries/eosiolib/core/eosio/crypto_ext.hpp)

The header declares following basic plain C functions which implement the core functionality:

## Plain C API

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


Also, the header contains a set of handy C++ wrappers to simplify the contracts code.

## C++ API

`ec_point` type was added to represent `G1` and `G2` points on the elliptic curve. 
`ec_point_view` type was added to provide a read-only interface for access to `G1` and `G2` points data.

### Data Types

```c++
   // Abstracts mutable G1 and G2 points
   template <std::size_t Size = 32>
   struct ec_point {
      std::vector<char> x; // x coordinate
      std::vector<char> y; // y coordinate
      // Construct from two coordinates represented by big integers 
      ec_point(std::vector<char>& x_, std::vector<char>& y_);
      // Construct from a serialized point
      ec_point(std::vector<char>& p);
      // Return a serialized point containing only x and y
      std::vector<char> serialized() const;
   };
 
   // Abstracts read-only G1 and G2 points    
   template <std::size_t Size = 32>
   struct ec_point_view {
      const char* x;
      const char* y;
      // Number of bytes in each of x and y       
      uint32_t size;
      // Construct from two coordinates. Their sizes must be equal      
      ec_point_view(const char* x_, uint32_t x_size, const char* y_, uint32_t y_size);
      // Construct from a serialized point
      ec_point_view(const std::vector<char>& p);
      // Construct a point view from a point
      ec_point_view(const ec_point<Size>& p);
      // Return serialized point containing only x and y
      std::vector<char> serialized() const;
   };
    
   // Big integer.
   using bigint = std::vector<char>;   
```

### Methods

- Addition operation on the elliptic curve `alt_bn128`
```c++
    template <typename T>
    g1_point alt_bn128_add( const T& op1, const T& op2 )
```

- Scalar multiplication operation on the elliptic curve `alt_bn128`
```c++
   template <typename T>
   g1_point alt_bn128_mul( const T& g1, const bigint& scalar)
```

- Optimal-Ate pairing check elliptic curve `alt_bn128`
```c++
   template <typename G1_T, typename G2_T>
   int32_t alt_bn128_pair( const std::vector<std::pair<G1_T, G2_T>>& pairs )
```

- Big integer modular exponentiation returns `( BASE^EXP ) % MOD`
```c++
   int32_t mod_exp( const bigint& base, const bigint& exp, const bigint& mod, bigint& result )
```

Please take a look into a [crypto_ext.hpp](https://github.com/AntelopeIO/cdt/blob/main/libraries/eosiolib/core/eosio/crypto_ext.hpp)
header file, it contains more wrappers and helper functions which may be useful in your code. 

### Examples

- `alt_bn128_add`
```c++
   std::vector<char> x1, y1, x2, y2; // Declare coordinates for points on an elliptic curve

   // Create the points on the curve 
   eosio::g1_point point1 {x1, y1};
   eosio::g1_point point2 {x2, y2};
   // Add two points and get a third point on the curve as result
   auto result = eosio::alt_bn128_add(point1, point2);

   // Do the same addition but with g1_point_view data types 
   eosio::g1_point_view point_view1 {x1.data(), x1.size(), y1.data(), y1.size()};
   eosio::g1_point_view point_view2 {x2.data(), x2.size(), y2.data(), y2.size()};
   result = eosio::alt_bn128_add(point_view1, point_view2);
```

- `alt_bn128_mul`
```c++
   // Declare coordinates for a point on an elliptic curve and a big integer 
   std::vector<char> x, y, scalar;
   eosio::bigint s {scalar};

   // Create the point object with given coordinates
   eosio::g1_point g1_point {x, y};
   // Multiply the point with a big integer and get a point on the curve as result
   auto result = eosio::alt_bn128_mul(g1_point, s);

   // Do the same multiplication but with g1_point_view data type
   eosio::g1_point_view g1_view {x.data(), x.size(), y.data(), y.size()};
   result = eosio::alt_bn128_mul(g1_view, s);
```

- `alt_bn128_pair`
```c++
   // Declare coordinates for the pairs of points on an elliptic curve 
   std::vector<char> g1_a_x, g1_a_y, g2_a_x, g2_a_y, g1_b_x, g1_b_y, g2_b_x, g2_b_y;


   // Create the point object by given coordinates
   // First pair
   eosio::g1_point g1_a {g1_a_x, g1_a_y};
   eosio::g2_point g2_a {g2_a_x, g2_a_y};
   // Second pair
   eosio::g1_point g1_b {g1_b_x, g1_b_y};
   eosio::g2_point g2_b {g2_b_x, g2_b_y};
   
   // Create the pairs object
   std::vector<std::pair<eosio::g1_point, eosio::g2_point>> pairs { {g1_a, g2_a}, {g1_b, g2_b} };
   
   // Perform the pairing check
   // Return:
   // -1 if there is an error 
   // 1 if the points not in a target group 
   // 0 if the points in a target group
   auto result = eosio::alt_bn128_pair(pairs);

   // Do the same pairing check but with g1_point_view and g2_point_view data types
   eosio::g1_point_view g1_view_a {g1_a_x.data(), g1_a_x.size(), g1_a_y.data(), g1_a_y.size()};
   eosio::g2_point_view g2_view_a {g2_a_x.data(), g2_a_x.size(), g2_a_y.data(), g2_a_y.size()};
   eosio::g1_point_view g1_view_b {g1_b_x.data(), g1_b_x.size(), g1_b_y.data(), g1_b_y.size()};
   eosio::g2_point_view g2_view_b {g2_b_x.data(), g2_b_x.size(), g2_b_y.data(), g2_b_y.size()};
   std::vector<std::pair<eosio::g1_point_view, eosio::g2_point_view>> view_pairs { {g1_a, g2_a}, {g1_b, g2_b} };
   result = eosio::alt_bn128_pair(view_pairs);
```

- `mod_exp`
```c++
   // Declare big integer variables for the base, the exponent and the modulo
   std::vector<char> base, exp, modulo;
   eosio::bigint base_val {base};
   eosio::bigint exp_val {exp};
   eosio::bigint modulo_val {modulo};
   // Declare and init a big integer variable for the result of exponentiation
   eosio::bigint result( modulo.size(), '\0' );
   // Perform the exponentiation
   // return -1 if there is an error otherwise 0
   auto rc = eosio::mod_exp(base_val, exp_val, modulo_val, result);
```
