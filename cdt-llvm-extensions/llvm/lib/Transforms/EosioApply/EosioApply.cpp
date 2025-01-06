//===- EosioApply ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Pass.h"
#include "llvm/IR/Attributes.h"
// Library's needed for new PassManager
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <set>
#include <string>
#include <utility>

using namespace llvm;

#define DEBUG_TYPE "eosio_apply"
static cl::opt<std::string> entry_opt (
   "entry",
   cl::desc("Specify entry point")
);

namespace {
  // EosioApply - Mutate the apply function as needed
  struct EosioApplyPass : public PassInfoMixin<FunctionListerPass> {

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
       if (F.hasFnAttribute("eosio_wasm_entry") || F.getName().equals("apply")) {
         outs() << "Running Eosio Apply Pass on: " << F.getName() << '\n';
         auto wasm_ctors = F.getParent()->getOrInsertFunction("__wasm_call_ctors", AttributeList{}, Type::getVoidTy(F.getContext()));
         auto wasm_dtors = F.getParent()->getOrInsertFunction("__cxa_finalize", AttributeList{}, Type::getVoidTy(F.getContext()), Type::getInt32Ty(F.getContext()));

         IRBuilder<> builder(&F.getEntryBlock());
         builder.SetInsertPoint(&(F.getEntryBlock().front()));

         auto set_contract = F.getParent()->getOrInsertFunction("eosio_set_contract_name", AttributeList{}, Type::getVoidTy(F.getContext()), Type::getInt64Ty(F.getContext()));

         CallInst* set_contract_call = builder.CreateCall(set_contract, {F.arg_begin()}, "");
         if (const Function* F_ = dyn_cast<const Function>(set_contract.getCallee()->stripPointerCasts()))
            set_contract_call->setCallingConv(F_->getCallingConv());

         CallInst* wasm_ctor_call = builder.CreateCall(wasm_ctors, {}, "");
         if (const Function* F_ = dyn_cast<const Function>(wasm_ctors.getCallee()->stripPointerCasts()))
            wasm_ctor_call->setCallingConv(F_->getCallingConv());

         for ( Function::iterator bb = F.begin(); bb != F.end(); bb++ ) {
            if (isa<ReturnInst>((*bb).getTerminator())) {
               builder.SetInsertPoint((*bb).getTerminator());
               // for now just call with null
               CallInst* wasm_dtor_call = builder.CreateCall(wasm_dtors, {Constant::getNullValue(Type::getInt32Ty(F.getContext()))}, "");
               if (const Function* F_ = dyn_cast<Function>(wasm_dtors.getCallee()->stripPointerCasts()))
                  wasm_dtor_call->setCallingConv(F_->getCallingConv());
            }
         }

         return PreservedAnalyses::all();
      }
    }
  };
}

PassPluginLibraryInfo getPassPluginInfo()
{
  const auto callback = [](PassBuilder &PB)
  {
    PB.registerPipelineStartEPCallback(
        [&](ModulePassManager &MPM, auto)
        {
          MPM.addPass(createModuleToFunctionPassAdaptor(FunctionListerPass()));
          return true;
        });
  };

  return {LLVM_PLUGIN_API_VERSION, "name", "0.0.1", callback};
};

/* When a plugin is loaded by the driver, it will call this entry point to
obtain information about this plugin and about how to register its passes.
*/
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo()
{
  return getPassPluginInfo();
}
