#include <eosio/call.hpp>
#include <eosio/eosio.hpp>

struct person_info {
   std::string first_name;
   std::string street;
};

class [[eosio::contract]] sync_call_addr_book_callee : public eosio::contract {
public:
   sync_call_addr_book_callee(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds): contract(receiver, code, ds) {}

   [[eosio::call]]
   void upsert(eosio::name user, std::string first_name, std::string street);

   [[eosio::call]]
   person_info get(eosio::name user);

   using upsert_read_only_func = eosio::call_wrapper<"upsert"_n, &sync_call_addr_book_callee::upsert, eosio::access_mode::read_only>;
   using upsert_func = eosio::call_wrapper<"upsert"_n, &sync_call_addr_book_callee::upsert>;
   using get_func = eosio::call_wrapper<"get"_n, &sync_call_addr_book_callee::get>;

private:
   struct [[eosio::table]] person {
      eosio::name key;
      std::string first_name;
      std::string street;

      uint64_t primary_key() const { return key.value; }
   };

   using address_index = eosio::multi_index<"people"_n, person>;
};
