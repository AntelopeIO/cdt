#include <eosio/eosio.hpp>
#include <eosio/transaction.hpp>
#include <eosio/name.hpp>

using namespace eosio;

class [[eosio::contract]] array_tests : public contract {
   public:
      using contract::contract;

   TABLE tests {
      uint64_t                    id;
      std::array<uint8_t,32>      str;
      uint64_t primary_key()      const { return id; }
   };

   struct info {
      int age;
      std::string name;
   };

   typedef multi_index<name("tests"), tests> tests_table;
   typedef std::array<std::string,4> array_string_4;
   struct my_struct {
      uint32_t id;
      std::array<array_string_4,2> aastr;

      bool operator==(const my_struct& b) const {
         return id == b.id &&
                aastr == b.aastr;
      }
   };

   // test inside using std::array
   [[eosio::action]]
   void testin(std::string message) {
      tests_table _tests(get_self(), get_self().value);

      std::array<uint8_t, 32> str = {'a','a','a','a','a','a','a','a',
                                        'a','a','a','a','a','a','a','a',
                                        'a','a','a','a','a','a','a','a',
                                        'a','a','a','a','a','a','a','a' };
      int len =  message.length() < 32 ? message.length() : 32;
      for(int i = 0; i < len ; ++i){
         str[i] = (uint8_t)message[i];
      }

      std::array<uint8_t, 32> str2 = str;
      eosio::cout << "size of std::array str is : " << str.size() << "\n";
      for(int i = 0; i < 32; ++i){
         eosio::cout << str[i] << " ";
      }
      eosio::cout << "\n";
      for(int i = 0; i < 32; ++i){
         eosio::cout << str2[i] << " ";
      }
      eosio::cout << "\n";
      std::array<info, 2> info_arr;
      info_arr[0].age = 20;
      info_arr[0].name = "abc";
      info_arr[1].age = 21;
      info_arr[1].name = "cde";
      for(int i = 0; i < 2; ++i){
         eosio::cout << info_arr[i].age << " " << info_arr[i].name << "\n";
      }
   }

   // test parameter using std::array
   [[eosio::action]]
   void testpa(std::array<int,4> input){
      std::array<int,4> arr = input;
      for(int i = 0; i < 4; ++i){
         eosio::cout << arr[i] << " ";
      }
      eosio::cout << "\n";
   }

   // test parameter and return value using std::array
   [[eosio::action]]
   // cleos -v push action eosio testre '[[1,2,3,4]]' -p eosio@active
   std::array<int,4> testre(std::array<int,4> input){
      std::array<int,4> arr = input;
      for(auto & v : arr) v += 1;
      return arr;
   }

   // test return value using std::array
   [[eosio::action]]
   // cleos -v push action eosio testre2 '[[1,2,3,4]]' -p eosio@active
   std::array<int,4> testre2(std::vector<int> input){
      std::array<int,4> arr;
      for(size_t i=0; i<4; ++i) arr[i] = input[i+1];
      return arr;
   }

   // test return using std::vector
   [[eosio::action]]
   // cleos -v push action eosio testrev '[[1,2,3,4]]' -p eosio@active
   std::vector<int> testrev(std::vector<int> input){
      std::vector<int> vec = input;
      for(auto & v : vec) v += 1;
      return vec;
   }

   // test nested array
   [[eosio::action]]
   void testne() {
      std::array<tests,2> nest;
      std::array<uint8_t, 32> str = {'a','a','a','a','a','a','a','a',
                                     'a','a','a','a','a','a','a','a',
                                     'a','a','a','a','a','a','a','a',
                                     'a','a','a','a','a','a','a','a' };

      nest[0].id = 1;
      nest[0].str = str;
      nest[1].id = 2;
      nest[1].str = str;
      for(int i = 0; i < nest.size(); ++i){
         eosio::cout << nest[i].id << "   " ;
         for(int j = 0; j < nest[i].str.size(); ++j) {
            eosio::cout << nest[i].str[j] + i << " ";
         }
         eosio::cout << "\n";
      }
      std::array<std::array<std::string, 5>, 3> nest2;
      for(int i = 0; i < nest2.size(); ++i){
         for(int j = 0; j < nest2[i].size(); ++j) {
            nest2[i][j] = "test nested ";
            eosio::cout << nest2[i][j] << " ";
         }
         eosio::cout << "\n";
      }
   }

   // test complex data
   [[eosio::action]]
   void testcom(name user) {
      require_auth(user);
      tests_table _tests(get_self(), get_self().value);

      std::array<uint8_t, 32> str = {'a','a','a','a','a','a','a','a',
                                     'a','a','a','a','a','a','a','a',
                                     'a','a','a','a','a','a','a','a',
                                     'a','a','a','a','a','a','a','a' };
      _tests.emplace(user, [&](auto& t) {
         t.id = user.value + std::time(0);  // primary key can't be same
         t.str = str;
      });
      auto it = _tests.begin();
      auto ite = _tests.end();
      while(it != ite){
         eosio::cout << "id = " << it->id << "\n";
         for(int i = 0; i < it->str.size(); ++i) {
            eosio::cout << it->str[i] << " ";
         }
         eosio::cout << "\n";
         ++it;
      }
   }

};
