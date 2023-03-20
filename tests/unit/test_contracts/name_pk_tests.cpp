// Verifies that a table with name-typed primary key works

#include <eosio/multi_index.hpp>
#include <eosio/contract.hpp>

struct [[eosio::table]] name_table {
    eosio::name pk;
    int num;

    auto primary_key() const { return pk; }
};
using name_table_idx = eosio::multi_index<"name.pk"_n, name_table>;

class [[eosio::contract]] name_pk_tests : public eosio::contract {
 public:
   using eosio::contract::contract;

   [[eosio::action]] void write() {
       name_table_idx table(get_self(), 0);
       table.emplace(get_self(), [](auto& row) {
           row.pk = "alice"_n;
           row.num = 2;
       });
       table.emplace(get_self(), [](auto& row) {
           row.pk = "bob"_n;
           row.num = 1;
       });
   }

   [[eosio::action]] void read() {
       name_table_idx table(get_self(), 0);
       eosio::check(table.get("alice"_n).num == 2, "num mismatch");
       eosio::check(table.get("bob"_n).num == 1, "num mismatch");
   }
};
