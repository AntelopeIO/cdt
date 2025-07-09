#include <eosio/call.hpp>
#include <eosio/eosio.hpp>

// Test the validation of the types of arguments passed in call_wrapper.
// Expected error:
// build/bin/../include/eosiolib/contracts/eosio/detail.hpp:60:7: error: static_assert failed due to requirement 'detail::is_same<sync_call_invalid_arg_nums::empty &, unsigned int>::value'
//   static_assert(detail::is_same<typename convert<T>::type, typename convert<typename std::tuple_element<I, deduced<Function>>::type>::type>::value);
class [[eosio::contract]] sync_call_invalid_arg_nums : public eosio::contract{
public:
   using contract::contract;

   [[eosio::call]]
   uint32_t sum(uint32_t a, uint32_t b, uint32_t c) {
      return a + b + c;
   }

   using sum_func = eosio::call_wrapper<"sum"_n, &sync_call_invalid_arg_nums::sum>;

   struct empty {
   };

   // Invalid first argument
   [[eosio::action]]
   void wrongrettype() {
      empty bad;
      sum_func{"callee"_n}(bad, 2, 3);
   }
};
