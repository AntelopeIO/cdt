#include <boost/test/unit_test.hpp>

#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <fc/variant_object.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;
using namespace fc;

using mvo = fc::mutable_variant_object;

namespace eosio::chain {
struct finalizer_authority {

    std::string   description;
    uint64_t      fweight = 0; // weight that this finalizer's vote has for meeting fthreshold
    std::array<unsigned char, 144> public_key;

};

} // eosio::chain

BOOST_AUTO_TEST_SUITE(set_finalizers_tests)

BOOST_FIXTURE_TEST_CASE(set_finalizers_test, tester) try {
    create_accounts( { "test"_n } );
    produce_block();

    set_code( config::system_account_name,  contracts::set_finalizers_test_wasm() );
    set_abi(  config::system_account_name,  contracts::set_finalizers_test_abi().data() );

    signed_block_ptr cur_block = produce_block();
    std::optional<block_header_extension> ext = cur_block->extract_header_extension(hs_finalizer_set_extension::extension_id());
    BOOST_ASSERT(!ext);

    const std::vector<uint8_t> G1 = {0x16, 0x0c, 0x53, 0xfd, 0x90, 0x87, 0xb3, 0x5c, 0xf5, 0xff, 0x76, 0x99, 0x67, 0xfc, 0x17, 0x78, 0xc1, 0xa1, 0x3b, 0x14, 0xc7, 0x95, 0x4f, 0x15, 0x47, 0xe7, 0xd0, 0xf3, 0xcd, 0x6a, 0xae, 0xf0, 0x40, 0xf4, 0xdb, 0x21, 0xcc, 0x6e, 0xce, 0xed, 0x75, 0xfb, 0x0b, 0x9e, 0x41, 0x77, 0x01, 0x12, 0x3a, 0x88, 0x18, 0xf3, 0x2a, 0x6c, 0x52, 0xff, 0x70, 0x02, 0x3b, 0x38, 0xe4, 0x9c, 0x89, 0x92, 0x55, 0xd0, 0xa9, 0x9f, 0x8d, 0x73, 0xd7, 0x89, 0x2a, 0xc1, 0x44, 0xa3, 0x5b, 0xf3, 0xca, 0x12, 0x17, 0x53, 0x4b, 0x96, 0x76, 0x1b, 0xff, 0x3c, 0x30, 0x44, 0x77, 0xe9, 0xed, 0xd2, 0x44, 0x0e, 0xfd, 0xff, 0x02, 0x00, 0x00, 0x00, 0x09, 0x76, 0x02, 0x00, 0x0c, 0xc4, 0x0b, 0x00, 0xf4, 0xeb, 0xba, 0x58, 0xc7, 0x53, 0x57, 0x98, 0x48, 0x5f, 0x45, 0x57, 0x52, 0x70, 0x53, 0x58, 0xce, 0x77, 0x6d, 0xec, 0x56, 0xa2, 0x97, 0x1a, 0x07, 0x5c, 0x93, 0xe4, 0x80, 0xfa, 0xc3, 0x5e, 0xf6, 0x15};

    push_action(config::system_account_name, "setfinal"_n, "test"_n, mvo()
        ("fin_set", mvo()("fthreshold", 1)
                         ("finalizers", std::vector<mvo>{mvo()
                                        ("description", "test_desc")
                                        ("fweight", 1)
                                        ("public_key_g1_jacobian", *reinterpret_cast<const std::vector<char>*>(&G1))})));
    cur_block = produce_block();
    ext = cur_block->extract_header_extension(hs_finalizer_set_extension::extension_id());
    BOOST_ASSERT(ext && std::get<hs_finalizer_set_extension>(*ext).finalizers.size() == 1 );

    // testing wrong public key size
    BOOST_CHECK_THROW(push_action(config::system_account_name, "setfinal"_n, "test"_n, mvo()
        ("fin_set", mvo()("fthreshold", 1)
                         ("finalizers", std::vector<mvo>{mvo()
                                        ("description", "test_desc")
                                        ("fweight", 1)
                                        ("public_key_g1_jacobian", std::vector<char>{'a', 'b', 'c'})}))), fc::exception);

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()