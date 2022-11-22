#include <eosio/eosio.hpp>
#include <eosio/unordered_map.hpp>

struct test_record {
   int pk;
   float s;
   std::string n;
};

class [[eosio::contract]] kv_map_tests : public eosio::contract {
public:
   using contract::contract;

   using testmap_t = eosio::unordered_map<"testmap"_n, int, float>;
   using testmap2_t = eosio::unordered_map<"testmap2"_n, std::string, std::string>;
   using testmap3_t = eosio::unordered_map<"testmap3"_n, int, float>;

   [[eosio::action]]
   void test() {
      testmap_t t = { "kvtest"_n, "kvtest"_n, {{33, 23.4f}, {10, 13.44f}, {103, 334.3f}} };

      auto p = t[33];

      p = 102.23; // note here this will update the held value and do a db set

      testmap_t t2{"kvtest"_n, "kvtest"_n};

      eosio::check(p == 102.23f, "should be the same value");
      eosio::check(p == t.at(33), "should be the same value");

      auto it = t.begin();

      auto& el = *it;

      eosio::check(el.value == 13.44f, "should still be the same from before");

      testmap2_t t3 = { "kvtest"_n, "kvtest"_n, {{"eosio", "fast"}, {"bit...", "hmm"}} };

      auto it2 = t3.begin();
      auto& el2 = *it2;

      ++it2;
      ++it2;
      auto it3 = std::move(it2);

      it2 = t3.end();

      eosio::check(it2 == it3, "they should be at the end and pointing to the same thing");
      eosio::check(it2 == t3.end(), "iterator should be at end");
      eosio::check(it3 == t3.end(), "iterator should be at end");
   }

   [[eosio::action]]
   void iter() {
      testmap_t t = {"kvtest"_n, "kvtest"_n, {{34, 23.4f}, {11, 13.44f}, {104, 334.3f}, {5, 33.42f}} };

      //called after test() and contains more data
      std::map<int, float> test_vals = {{34, 23.4f}, {11, 13.44f}, {104, 334.3f}, {5, 33.42f}, {33, 102.23f}, {10, 13.44f}, {103, 334.3f}};

      int i = 0;

      // test that this will work with auto ranged for loop
      for ( const auto& e : t ) {
         ++i;
         eosio::check(test_vals.find(e.key)->second == e.value, "invalid value in iter test");
      }
      eosio::check(t.size() == 7, "size is wrong");
      eosio::check(i == 7, "range based loop iterates less elements than there are in container");
   }

   [[eosio::action]]
   void erase() {
      testmap_t t("kvtest"_n, "kvtest"_n);

      t.contains(34);
      t.erase(34);

      eosio::check(!t.contains(34), "should have erased a value");
   }

   [[eosio::action]]
   void eraseexcp() {
      testmap_t t("kvtest"_n, "kvtest"_n);
      t.at(34); // this should cause an assertion
   }

   [[eosio::action]]
   void bounds() {
      testmap3_t t = {"kvtest"_n, "kvtest"_n, {{33, 10}, {10, 41.2f}, {11, 100.100f}, {2, 17.42f}}};

      auto it = t.lower_bound(11);

      eosio::check(it->key == 11, "should be pointing to 11");

      it = t.lower_bound(31);

      eosio::check(it->key == 33, "should be pointing to 33");

      it = t.lower_bound(36);

      eosio::check(it == t.end(), "should be pointing to end");

      auto it2 = t.lower_bound(1);

      eosio::check(it2->key == 2, "should be pointing to 2");

      it = t.upper_bound(10);

      eosio::check(it->key == 11, "should be pointing to 11");

      it = t.upper_bound(33);

      eosio::check(it == t.end(), "should be pointing to end");
   }

   [[eosio::action]]
   void ranges() {
      testmap3_t t = {"kvtest"_n, "kvtest"_n, {{17, 9.9f}}};

      auto range = t.equal_range(16);

      eosio::check(range.first->key == 17, "should be pointing to 17");
      eosio::check(range.second->key == 17, "should be pointing to 17");

      range = t.equal_range(1);

      eosio::check(range.first->key == 2, "should be pointing to 2");
      eosio::check(range.second->key == 2, "should be pointing to 2");
   }

   [[eosio::action]]
   void empty() {
      testmap_t t("kvtest"_n, "kvtest"_n);
      eosio::check(!t.empty(), "should be not empty");
      t.erase(t.begin(), t.end());
      eosio::check(t.empty(), "should be empty");
   }
};
