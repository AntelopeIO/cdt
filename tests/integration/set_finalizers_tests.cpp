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

    push_action(config::system_account_name, "setfinal"_n, "test"_n, mvo()
        ("fin_set", mvo()("fthreshold", 1)
                         ("finalizers", std::vector<mvo>{mvo()
                                        ("description", "test_desc")
                                        ("fweight", 1)
                                        ("public_key_g1_affine_le", "220ef5c49c1868e85b82658df9766fc2cee4bd0b5d9880d4cdee9139edd3c4ebdae4074f1d3db3f3c9213962942eef091b00d9f2a9b837015c8dbe507f242aee459272589b6b2973bcf33eb4608722c70c1ac3d1ade40c845b9e15ea6380080a")})));
    signed_block_ptr cur_block = produce_block();
    fc::variant pretty_output;
    abi_serializer::to_variant( *cur_block, pretty_output, get_resolver(), fc::microseconds::maximum() );
    BOOST_REQUIRE(pretty_output.get_object().contains("proposed_finalizer_policy"));
    BOOST_REQUIRE_EQUAL(pretty_output["proposed_finalizer_policy"]["generation"], 1);
    BOOST_REQUIRE_EQUAL(pretty_output["proposed_finalizer_policy"]["fthreshold"], 1);
    BOOST_REQUIRE_EQUAL(pretty_output["proposed_finalizer_policy"]["finalizers"].size(), 1u);
    BOOST_REQUIRE_EQUAL(pretty_output["proposed_finalizer_policy"]["finalizers"][size_t(0)]["description"], "test_desc");
    BOOST_REQUIRE_EQUAL(pretty_output["proposed_finalizer_policy"]["finalizers"][size_t(0)]["fweight"], 1);
    BOOST_REQUIRE_EQUAL(pretty_output["proposed_finalizer_policy"]["finalizers"][size_t(0)]["public_key"], "PUB_BLS_Ig71xJwYaOhbgmWN+XZvws7kvQtdmIDUze6ROe3TxOva5AdPHT2z88khOWKULu8JGwDZ8qm4NwFcjb5QfyQq7kWSclibaylzvPM+tGCHIscMGsPRreQMhFueFepjgAgKXjOb8g==");

    // testing wrong public key size
    BOOST_CHECK_THROW(push_action(config::system_account_name, "setfinal"_n, "test"_n, mvo()
        ("fin_set", mvo()("fthreshold", 1)
                         ("finalizers", std::vector<mvo>{mvo()
                                        ("description", "test_desc")
                                        ("fweight", 1)
                                        ("public_key_g1_affine_le", std::vector<char>{'a', 'b', 'c'})}))), fc::exception);

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
