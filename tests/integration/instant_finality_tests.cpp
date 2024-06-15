#include <boost/test/unit_test.hpp>

#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <fc/variant_object.hpp>

#include <contracts.hpp>

using namespace eosio;
using namespace eosio::testing;
using namespace fc;

using mvo = fc::mutable_variant_object;

BOOST_AUTO_TEST_SUITE(instant_finality_tests)

BOOST_FIXTURE_TEST_CASE(instant_finality_test, tester) try {
    create_accounts( { "test"_n } );
    produce_block();

    set_code( config::system_account_name,  contracts::instant_finality_test_wasm() );
    set_abi(  config::system_account_name,  contracts::instant_finality_test_abi().data() );

    produce_block();

    push_action(config::system_account_name, "setfinalizer"_n, "test"_n, mvo()
        ("finalizer_policy", mvo()("threshold", 1)
        ("finalizers", std::vector<mvo>{mvo()
        ("description", "test_desc")
        ("weight", 1)
        ("public_key", "744beeb74c9d1debc318fe847f73b822ae905dff6351c3144f59c22515fe251625158acefea2adff1e37f6f509d83919df639de8074967e4bd756444f52cbeed0e9b363a6820e3f4716ce4282d43aa685f137a5a5be293840de7a0b915f12b08")})));
    signed_block_ptr cur_block = produce_block();
    fc::variant pretty_output;
    abi_serializer::to_variant( *cur_block, pretty_output, get_resolver(), fc::microseconds::maximum() );
    std::cout << fc::json::to_string(pretty_output, fc::time_point::now() + abi_serializer_max_time) << std::endl;
    BOOST_REQUIRE(pretty_output.get_object().contains("instant_finality_extension"));

    std::string output_json = fc::json::to_pretty_string(pretty_output);
    BOOST_TEST(output_json.find("\"generation\": 2") != std::string::npos);
    BOOST_TEST(output_json.find("\"threshold\": 1") != std::string::npos);
    BOOST_TEST(output_json.find("\"description\": \"test_desc\"") != std::string::npos);
    BOOST_TEST(output_json.find("\"weight\": 1") != std::string::npos);
    BOOST_TEST(output_json.find("PUB_BLS_dEvut0ydHevDGP6Ef3O4Iq6QXf9jUcMUT1nCJRX-JRYlFYrO_qKt_x439vUJ2DkZ32Od6AdJZ-S9dWRE9Sy-7Q6bNjpoIOP0cWzkKC1DqmhfE3paW-KThA3noLkV8SsILcfxpQ") != std::string::npos);

    // testing wrong public key size
    BOOST_CHECK_THROW(push_action(config::system_account_name, "setfinalizer"_n, "test"_n, mvo()
        ("finalizer_policy", mvo()("threshold", 1)
        ("finalizers", std::vector<mvo>{mvo()
        ("description", "test_desc")
        ("weight", 1)
        ("public_key", std::vector<char>{'a', 'b', 'c'})}))), fc::exception);

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
