#include <eosio/eosio.hpp>

using namespace eosio;

template<uint64_t Tag>
struct TaggedNumber {
    uint64_t value;
};

class [[eosio::contract]] tagged_number_test : public contract {
  public:
      using contract::contract;
      
      [[eosio::action]]
      void test(TaggedNumber<"a.tag"_n.value>) {
      }
};
