#include <boost/test/unit_test.hpp>

#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <fc/variant_object.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;
using namespace fc;

using mvo = fc::mutable_variant_object;

BOOST_AUTO_TEST_SUITE(set_finalizers_tests)

BOOST_FIXTURE_TEST_CASE(set_finalizers_test, tester) try {
    create_accounts( { "test"_n } );
    produce_block();

    set_code( config::system_account_name,  contracts::set_finalizers_test_wasm() );
    set_abi(  config::system_account_name,  contracts::set_finalizers_test_abi().data() );

    produce_block();

    const std::vector<uint8_t> G1 = {0x22, 0x0e, 0xf5, 0xc4, 0x9c, 0x18, 0x68, 0xe8,
                                     0x5b, 0x82, 0x65, 0x8d, 0xf9, 0x76, 0x6f, 0xc2,
                                     0xce, 0xe4, 0xbd, 0x0b, 0x5d, 0x98, 0x80, 0xd4,
                                     0xcd, 0xee, 0x91, 0x39, 0xed, 0xd3, 0xc4, 0xeb,
                                     0xda, 0xe4, 0x07, 0x4f, 0x1d, 0x3d, 0xb3, 0xf3, 
                                     0xc9, 0x21, 0x39, 0x62, 0x94, 0x2e, 0xef, 0x09,
                                     0x1b, 0x00, 0xd9, 0xf2, 0xa9, 0xb8, 0x37, 0x01,
                                     0x5c, 0x8d, 0xbe, 0x50, 0x7f, 0x24, 0x2a, 0xee,
                                     0x45, 0x92, 0x72, 0x58, 0x9b, 0x6b, 0x29, 0x73,
                                     0xbc, 0xf3, 0x3e, 0xb4, 0x60, 0x87, 0x22, 0xc7,
                                     0x0c, 0x1a, 0xc3, 0xd1, 0xad, 0xe4, 0x0c, 0x84,
                                     0x5b, 0x9e, 0x15, 0xea, 0x63, 0x80, 0x08, 0x0a}; // 12x8 = 96 bytes
    
    push_action(config::system_account_name, "setfinal"_n, "test"_n, mvo()
        ("fin_set", mvo()("fthreshold", 1)
                         ("finalizers", std::vector<mvo>{mvo()
                                        ("description", "test_desc")
                                        ("fweight", 1)
                                        ("public_key_g1_affine_le", *reinterpret_cast<const std::vector<char>*>(&G1))})));
    signed_block_ptr cur_block = produce_block();
    fc::variant pretty_output;
    abi_serializer::to_variant( *cur_block, pretty_output, get_resolver(), fc::microseconds::maximum() );
    BOOST_ASSERT(pretty_output.get_object().contains("proposed_finalizer_set"));
    BOOST_ASSERT(pretty_output["proposed_finalizer_set"]["generation"] == 1);
    BOOST_ASSERT(pretty_output["proposed_finalizer_set"]["fthreshold"] == 1);
    BOOST_ASSERT(pretty_output["proposed_finalizer_set"]["finalizers"].size() == 1);
    BOOST_ASSERT(pretty_output["proposed_finalizer_set"]["finalizers"][size_t(0)]["description"] == "test_desc");
    BOOST_ASSERT(pretty_output["proposed_finalizer_set"]["finalizers"][size_t(0)]["fweight"] == 1);
    //TODO: add key check here after base64 support will be added

    // testing wrong public key size
    BOOST_CHECK_THROW(push_action(config::system_account_name, "setfinal"_n, "test"_n, mvo()
        ("fin_set", mvo()("fthreshold", 1)
                         ("finalizers", std::vector<mvo>{mvo()
                                        ("description", "test_desc")
                                        ("fweight", 1)
                                        ("public_key_g1_affine_le", std::vector<char>{'a', 'b', 'c'})}))), fc::exception);

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()