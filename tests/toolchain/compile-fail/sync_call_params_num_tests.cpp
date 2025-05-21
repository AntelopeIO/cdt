#include <eosio/call.hpp>
#include <eosio/eosio.hpp>

// Test the validation of the number of arguments passed in call_wrapper.
// Expected error:
// .../build/bin/../include/eosiolib/contracts/eosio/detail.hpp:72:7: error: static_assert failed due to requirement 'sizeof...(Ts) == std::tuple_size<std::__1::tuple<unsigned int, unsigned int, unsigned int>>::value'
//    static_assert(sizeof...(Ts) == std::tuple_size<deduced<Function>>::value);

class [[eosio::contract]] sync_call_invalid_arg_nums : public eosio::contract{
public:
   using contract::contract;

   [[eosio::call]]
   uint32_t sum(uint32_t a, uint32_t b, uint32_t c) {
      return a + b + c;
   }

   using sum_func = eosio::call_wrapper<"sum"_n, &sync_call_invalid_arg_nums::sum>;

   // Fewer number of arguments
   [[eosio::action]]
   void fewerargs() {
      sum_func{"callee"_n}(1, 2);
   }

   // More number of arguments
   [[eosio::action]]
   void moreargs() {
      sum_func{"callee"_n}(1, 2, 3, 4);
   }
};
