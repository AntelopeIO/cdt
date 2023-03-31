#include "options.hpp"
#include "file-utils.hpp"
#include "run.hpp"
#include "native-runner.hpp"
#include "wasm-runner.hpp"
#include "version.hpp"

#include <cassert>
#include <string.h>
#include <dlfcn.h>

#include <eosio/vm/backend.hpp>
#include <eosio/utils.hpp>

#include <native/eosio/crt.hpp>
#include <native/eosio/intrinsics.hpp>

#include "llvm/Support/CommandLine.h"

using namespace eosio;
using namespace eosio::cdt;
using namespace eosio::vm;
using namespace llvm;

int main(int argc, const char **argv) {

   cl::SetVersionPrinter([](llvm::raw_ostream& os) {
         os << "Antler-run version " << app_version() << "\n";
   });

   cl::ParseCommandLineOptions(argc, argv);
   
   const auto& contract_path = contract_path_opt.getValue();
   auto contract_type = utils::get_file_type(contract_path.c_str());

   if (test_so_opt) {
      if ( contract_type != utils::file_type::elf_shared_object ) {
         fprintf(stderr, "not a shared object file: %s\n", file_type_str(contract_type).c_str());
      }
      
      //loads shared object and throws if any export function is missing
      native::runner runner(contract_path);
      // TODO: add unit test that generates shared object and executes runner with this flag
      return 0;
   }

   const auto& node_url  = nodeos_url_opt.getValue();
   const auto& node_port = nodeos_port_opt.getValue();
   const auto& action    = eosio::name(action_name_opt.getValue());
   const auto& account   = eosio::name(register_opt.getValue());

   if (contract_type == utils::file_type::elf_shared_object) {
      testing::run( native::runner(contract_path), false, node_url, node_port, account );
   } else if (contract_type == utils::file_type::wasm) {
      testing::run( wasm::runner(contract_path), false, node_url, node_port, account );
   }

   return 0;
}