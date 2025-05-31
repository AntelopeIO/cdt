#include "sync_call_addr_book_callee.hpp"
#include <eosio/eosio.hpp>

using namespace eosio;

void sync_call_addr_book_callee::upsert(name user, std::string first_name, std::string street) {
   // Intentionally leave out require_auth(user) to test upsert cannot be called as a read_only
   // sync call

   address_index addresses(get_first_receiver(), get_first_receiver().value);
   auto iterator = addresses.find(user.value);
   if( iterator == addresses.end() )
   {
     addresses.emplace(user, [&]( auto& row ) {
      row.key = user;
      row.first_name = first_name;
      row.street = street;
     });
   }
   else {
     addresses.modify(iterator, user, [&]( auto& row ) {
       row.key = user;
       row.first_name = first_name;
      row.street = street;
     });
   }
}

person_info sync_call_addr_book_callee::get(name user) {
   address_index addresses(get_first_receiver(), get_first_receiver().value);

   auto iterator = addresses.find(user.value);
   check(iterator != addresses.end(), "Record does not exist");

   return person_info{ .first_name = iterator->first_name,
                       .street     = iterator->street };
}
