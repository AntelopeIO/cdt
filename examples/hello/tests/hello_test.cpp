#include <eosio/eosio.hpp>
#include <eosio/tester.hpp>

#include <hello.hpp>

using namespace eosio;
using namespace eosio::native;

using generic_intrinsic = void*(*)();

extern "C" {
   void initialize();
   void register_intrinsic(uint32_t type, generic_intrinsic fn);
}
// need to create a dispatcher, codegen will not be done for native builds until a later release
EOSIO_DISPATCH(hello, (hi)(check))

EOSIO_TEST_BEGIN(hello_test)

   initialize();
   
   // "Name : hello" should be in the print buffer
   CHECK_PRINT("Name : hello\n",
         []() {
            apply("test"_n.value, "test"_n.value, "hi"_n.value);
            });
           
   // should not assert
   apply("test"_n.value, "test"_n.value, "check"_n.value);
   
   name nm = "null"_n;
   intrinsics::set_intrinsic<intrinsics::read_action_data>(
         [&](void* m, uint32_t len) {
            check(len <= sizeof(eosio::name), "failed from read_action_data");
            *((eosio::name*)m) = nm;
            return len; 
         });

   REQUIRE_ASSERT( "check name not equal to `hello`",
         []() {
            // should assert
            apply("test"_n.value, "test"_n.value, "check"_n.value);
            });

EOSIO_TEST_END

void register_intrinsic(uint32_t type, generic_intrinsic fn) {
   switch (type) {
      case intrinsics::prints_l:
      intrinsics::set_intrinsic<intrinsics::prints_l>( reinterpret_cast<void* (*)(const char*, uint32_t)>(fn) );
      break;
      case intrinsics::prints:
      intrinsics::set_intrinsic<intrinsics::prints>( reinterpret_cast<void* (*)(const char*)>(fn) );
      break;
      case intrinsics::printn:
      intrinsics::set_intrinsic<intrinsics::printn>( reinterpret_cast<void* (*)(uint64_t)>(fn) );
      break;
   }
}

void initialize() {
   // These can be redefined by the user to suit there needs per unit test
   // the idea is that in a future release we will have a base library that 
   // initializes these to "useable" default implementations and probably 
   // helpers to more easily define read_action_data and action_data_size intrinsics
   // like these
   intrinsics::set_intrinsic<intrinsics::read_action_data>(
         [](void* m, uint32_t len) {
            check(len <= sizeof(eosio::name), "failed from read_action_data");
            *((eosio::name*)m) = "hello"_n;
            return len; 
         });

   intrinsics::set_intrinsic<intrinsics::action_data_size>(
         []() {
            return (uint32_t)sizeof(eosio::name);
         });
   
   // intrinsics::set_intrinsic<intrinsics::require_auth>(
   //       [](capi_name nm) {
   //       });
   // intrinsics::set_intrinsic<intrinsics::prints_l>(
   //    [](const char* cs, uint32_t l) {
   //       for (int i=0; i < l; i++)
   //          printf("%c", cs[i]);
   //       });
   // intrinsics::set_intrinsic<intrinsics::prints>(
   //    [](const char* cs) {
   //       for (int i=0; cs[i] != '\0'; i++)
   //          printf("%c", cs[i]);
   //       });
   // intrinsics::set_intrinsic<intrinsics::printn>(
   //    [](uint64_t nm) {
   //       std::string s = eosio::name(nm).to_string();
   //       prints_l(s.c_str(), s.length());
   //    });
}

// boilerplate main, this will be generated in a future release
int main(int argc, char** argv) {
   silence_output(false);
   EOSIO_TEST(hello_test);
   return has_failed();
}
