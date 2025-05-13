#include "sync_call_addr_book_callee.hpp"

#include <eosio/eosio.hpp>
#include <eosio/call.hpp>

class [[eosio::contract]] sync_call_addr_book_caller : public eosio::contract{
public:
   using contract::contract;

   // Insert an entry using read_only, which will fail
   [[eosio::action]]
   void upsertrdonly(eosio::name user, std::string first_name, std::string street) {
      sync_call_addr_book_callee::upsert_func{"callee"_n, eosio::execution_mode::read_only}(user, first_name, street);
   }

   // Insert an entry
   [[eosio::action]]
   void upsert(eosio::name user, std::string first_name, std::string street) {
      sync_call_addr_book_callee::upsert_func{"callee"_n}(user, first_name, street);
   }

   // Read an entry
   [[eosio::action]]
   person_info get(eosio::name user) {
      auto user_info = sync_call_addr_book_callee::get_func{"callee"_n}(user);
      eosio::check(user_info.first_name == "alice", "first name not alice");
      eosio::check(user_info.street == "123 Main St.", "street not 123 Main St.");
      return user_info;
   }
};
