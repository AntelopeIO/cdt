#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;

using mvo = fc::mutable_variant_object;

BOOST_AUTO_TEST_SUITE(crypto_primitives_tests)

BOOST_FIXTURE_TEST_CASE( sha3_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "sha3test"_n, "test"_n, mvo()
      ("val", "hello")
      ("sha3_dg", "3338be694f50c5f338814986cdf0686453a888b84f424d792af4b9202398f392"));

   push_action("eosio"_n, "sha3test"_n, "test"_n, mvo()
      ("val", "")
      ("sha3_dg", "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a"));

   push_action("eosio"_n, "sha3test"_n, "test"_n, mvo()
      ("val", "mandymandymandy")
      ("sha3_dg", "38cf98e66aaa9f1d93e5192f79a0f6da79be794914812cf29eec8409f670155d"));

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( keccak_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "keccaktest"_n, "test"_n, mvo()
      ("val", "hello")
      ("sha3_dg", "1c8aff950685c2ed4bc3174f3472287b56d9517b9c948127319a09a7a36deac8"));

   push_action("eosio"_n, "keccaktest"_n, "test"_n, mvo()
      ("val", "")
      ("sha3_dg", "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470"));

   push_action("eosio"_n, "keccaktest"_n, "test"_n, mvo()
      ("val", "mandymandymandy")
      ("sha3_dg", "b2fe07df24acbc89ebd3e4ed16d5348b6d5cbbcb22855e2af7a2945088dd94d5"));

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( bn128_add_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "addtest"_n, "test"_n, mvo()
      ("x1", "222480c9f95409bfa4ac6ae890b9c150bc88542b87b352e92950c340458b0c09")
      ("y1", "2976efd698cf23b414ea622b3f720dd9080d679042482ff3668cb2e32cad8ae2")
      ("x2", "1bd20beca3d8d28e536d2b5bd3bf36d76af68af5e6c96ca6e5519ba9ff8f5332")
      ("y2", "2a53edf6b48bcf5cb1c0b4ad1d36dfce06a79dcd6526f1c386a14d8ce4649844")
      ("expected_rc", 0)
      ("expected_x", "16c7c4042e3a725ddbacf197c519c3dcad2bc87dfd9ac7e1e1631154ee0b7d9c")
      ("expected_y", "19cd640dd28c9811ebaaa095a16b16190d08d6906c4f926fce581985fe35be0e"));

   // P1 not on curve
   push_action("eosio"_n, "addtest"_n, "test"_n, mvo()
      ("x1", "222480c9f95409bfa4ac6ae890b9c150bc88542b87b352e92950c340458b0c09")
      ("y1", "2976efd698cf23b414ea622b3f720dd9080d679042482ff3668cb2e32cad8ae2")
      ("x2", "2a53edf6b48bcf5cb1c0b4ad1d36dfce06a79dcd6526f1c386a14d8ce4649844")
      ("y2", "1bd20beca3d8d28e536d2b5bd3bf36d76af68af5e6c96ca6e5519ba9ff8f5332")
      ("expected_rc", -1)
      ("expected_x", "0000000000000000000000000000000000000000000000000000000000000000")
      ("expected_y", "0000000000000000000000000000000000000000000000000000000000000000"));

} FC_LOG_AND_RETHROW()

// Test G1 constructed from a serialized point
BOOST_FIXTURE_TEST_CASE( bn128_add_serialized_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "addtest1"_n, "test"_n, mvo()
      ("p1", "222480c9f95409bfa4ac6ae890b9c150bc88542b87b352e92950c340458b0c092976efd698cf23b414ea622b3f720dd9080d679042482ff3668cb2e32cad8ae2")
      ("p2", "1bd20beca3d8d28e536d2b5bd3bf36d76af68af5e6c96ca6e5519ba9ff8f53322a53edf6b48bcf5cb1c0b4ad1d36dfce06a79dcd6526f1c386a14d8ce4649844")
      ("expected_rc", 0)
      ("expected_x", "16c7c4042e3a725ddbacf197c519c3dcad2bc87dfd9ac7e1e1631154ee0b7d9c")
      ("expected_y", "19cd640dd28c9811ebaaa095a16b16190d08d6906c4f926fce581985fe35be0e"));

   // P1 not on curve
   push_action("eosio"_n, "addtest1"_n, "test"_n, mvo()
      ("p1", "222480c9f95409bfa4ac6ae890b9c150bc88542b87b352e92950c340458b0c092976efd698cf23b414ea622b3f720dd9080d679042482ff3668cb2e32cad8ae2")
      ("p2", "2a53edf6b48bcf5cb1c0b4ad1d36dfce06a79dcd6526f1c386a14d8ce46498441bd20beca3d8d28e536d2b5bd3bf36d76af68af5e6c96ca6e5519ba9ff8f5332")
      ("expected_rc", -1)
      ("expected_x", "0000000000000000000000000000000000000000000000000000000000000000")
      ("expected_y", "0000000000000000000000000000000000000000000000000000000000000000"));

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( bn128_mul_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "multest"_n, "test"_n, mvo()
      ("g1_x", "007c43fcd125b2b13e2521e395a81727710a46b34fe279adbf1b94c72f7f9136")
      ("g1_y", "0db2f980370fb8962751c6ff064f4516a6a93d563388518bb77ab9a6b30755be")
      ("scalar", "0312ed43559cf8ecbab5221256a56e567aac5035308e3f1d54954d8b97cd1c9b")
      ("expected_rc", 0)
      ("expected_x", "2d66cdeca5e1715896a5a924c50a149be87ddd2347b862150fbb0fd7d0b1833c")
      ("expected_y", "11c76319ebefc5379f7aa6d85d40169a612597637242a4bbb39e5cd3b844becd"));
} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( bn128_pair_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "pairtest"_n, "test"_n, mvo()
      ("g1_a_x", "0f25929bcb43d5a57391564615c9e70a992b10eafa4db109709649cf48c50dd2")
      ("g1_a_y", "16da2f5cb6be7a0aa72c440c53c9bbdfec6c36c7d515536431b3a865468acbba")
      ("g2_a_x", "2e89718ad33c8bed92e210e81d1853435399a271913a6520736a4729cf0d51eb01a9e2ffa2e92599b68e44de5bcf354fa2642bd4f26b259daa6f7ce3ed57aeb3")
      ("g2_a_y", "14a9a87b789a58af499b314e13c3d65bede56c07ea2d418d6874857b70763713178fb49a2d6cd347dc58973ff49613a20757d0fcc22079f9abd10c3baee24590")
      ("g1_b_x", "1b9e027bd5cfc2cb5db82d4dc9677ac795ec500ecd47deee3b5da006d6d049b8")
      ("g1_b_y", "11d7511c78158de484232fc68daf8a45cf217d1c2fae693ff5871e8752d73b21")
      ("g2_b_x", "198e9393920d483a7260bfb731fb5d25f1aa493335a9e71297e485b7aef312c21800deef121f1e76426a00665e5c4479674322d4f75edadd46debd5cd992f6ed")
      ("g2_b_y", "090689d0585ff075ec9e99ad690c3395bc4b313370b38ef355acdadcd122975b12c85ea5db8c6deb4aab71808dcb408fe3d1e7690c43d37b4ce6cc0166fa7daa")
      ("expected", 0));
} FC_LOG_AND_RETHROW()

// Test G1 and G2 constructed from serialized points
BOOST_FIXTURE_TEST_CASE( bn128_serialized_point_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "pairtest1"_n, "test"_n, mvo()
      ("g1a", "0f25929bcb43d5a57391564615c9e70a992b10eafa4db109709649cf48c50dd216da2f5cb6be7a0aa72c440c53c9bbdfec6c36c7d515536431b3a865468acbba")
      ("g2a", "2e89718ad33c8bed92e210e81d1853435399a271913a6520736a4729cf0d51eb01a9e2ffa2e92599b68e44de5bcf354fa2642bd4f26b259daa6f7ce3ed57aeb314a9a87b789a58af499b314e13c3d65bede56c07ea2d418d6874857b70763713178fb49a2d6cd347dc58973ff49613a20757d0fcc22079f9abd10c3baee24590")
      ("g1b", "1b9e027bd5cfc2cb5db82d4dc9677ac795ec500ecd47deee3b5da006d6d049b811d7511c78158de484232fc68daf8a45cf217d1c2fae693ff5871e8752d73b21")
      ("g2b", "198e9393920d483a7260bfb731fb5d25f1aa493335a9e71297e485b7aef312c21800deef121f1e76426a00665e5c4479674322d4f75edadd46debd5cd992f6ed090689d0585ff075ec9e99ad690c3395bc4b313370b38ef355acdadcd122975b12c85ea5db8c6deb4aab71808dcb408fe3d1e7690c43d37b4ce6cc0166fa7daa")
      ("expected", 0));
} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( modexp_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "modexptest"_n, "test"_n, mvo()
      ("base", "03")
      ("exp", "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2e")
      ("modulo", "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f")
      ("expected_rc", 0)
      ("expected_result", "0000000000000000000000000000000000000000000000000000000000000001"));

   push_action("eosio"_n, "modexptest"_n, "test"_n, mvo()
      ("base", "")
      ("exp", "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2e")
      ("modulo", "fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f")
      ("expected_rc", 0)
      ("expected_result", "0000000000000000000000000000000000000000000000000000000000000000"));

   push_action("eosio"_n, "modexptest"_n, "test"_n, mvo()
      ("base", "00")
      ("exp", "00")
      ("modulo", "0F")
      ("expected_rc", 0)
      ("expected_result", "01"));
} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( blake2f_tests, tester ) try {
   create_accounts( { "test"_n } );
   produce_block();

   set_code( "eosio"_n,  contracts::crypto_primitives_test_wasm() );
   set_abi(  "eosio"_n,  contracts::crypto_primitives_test_abi().data() );

   produce_blocks();

   push_action("eosio"_n, "blake2ftest"_n, "test"_n, mvo()
      ("rounds", 0)
      ("state", "48c9bdf267e6096a3ba7ca8485ae67bb2bf894fe72f36e3cf1361d5f3af54fa5d182e6ad7f520e511f6c3e2b8c68059b6bbd41fbabd9831f79217e1319cde05b")
      ("msg", "6162630000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
      ("t0_offset", "0300000000000000")
      ("t1_offset", "0000000000000000")
      ("final", true)
      ("expected_rc", 0)
      ("expected_result", "08c9bcf367e6096a3ba7ca8485ae67bb2bf894fe72f36e3cf1361d5f3af54fa5d282e6ad7f520e511f6c3e2b8c68059b9442be0454267ce079217e1319cde05b"));
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
