#pragma once

#include "llvm/Support/CommandLine.h"

#include <string>

static llvm::cl::OptionCategory AntlerRunOptions("Runner options");

llvm::cl::opt<std::string> contract_path_opt ( "contract", 
                                               llvm::cl::desc("Smart contract path to debug. It could be either wasm or native shared object"),
                                               llvm::cl::cat(AntlerRunOptions),
                                               llvm::cl::Required );
llvm::cl::opt<bool>        test_so_opt       ( "test", 
                                               llvm::cl::desc("Test that shared object is loadable and has neccesary exports"),
                                               llvm::cl::cat(AntlerRunOptions),
                                               llvm::cl::ZeroOrMore );
llvm::cl::opt<std::string> nodeos_url_opt    ( "nodeos-url", 
                                               llvm::cl::desc("`nodeos` URL"),
                                               llvm::cl::cat(AntlerRunOptions),
                                               llvm::cl::ZeroOrMore );
llvm::cl::opt<std::string> nodeos_port_opt   ( "nodeos-port", 
                                               llvm::cl::desc("`nodeos` port"),
                                               llvm::cl::cat(AntlerRunOptions),
                                               llvm::cl::ZeroOrMore );
llvm::cl::opt<std::string> action_name_opt   ( "call-action", 
                                               llvm::cl::desc("Action that will be called"),
                                               llvm::cl::cat(AntlerRunOptions),
                                               llvm::cl::ZeroOrMore );
llvm::cl::opt<std::string> register_opt      ( "register", 
                                               llvm::cl::desc("Register an account name to be debuggable through `nodeos`"),
                                               llvm::cl::cat(AntlerRunOptions),
                                               llvm::cl::ZeroOrMore );
