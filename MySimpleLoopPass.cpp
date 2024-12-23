//===- MySimpleLoopPass.cpp - Example Loop Pass (New PM) ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// A simple example of a loop pass using the New Pass Manager.
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/PassManager.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
  // New PM loop pass
  struct MySimpleLoopPass : PassInfoMixin<MySimpleLoopPass> {
    // The main entry point - the name signature is important:
    PreservedAnalyses run(Loop &L,
                          LoopAnalysisManager &LAM,
                          LoopStandardAnalysisResults &AR,
                          LPMUpdater &Updater) {
      // AR provides access to standard analyses like LoopInfo, DominatorTree, etc.
      // For example:
      // auto &LI = AR.LI;
      
      errs() << "Analyzing loop in function: "
             << L.getHeader()->getParent()->getName() << "\n";

      // Example: Count the number of blocks in the loop
      unsigned numBBs = 0;
      for (auto *BB : L.blocks()) {
        numBBs++;
      }
      errs() << "  This loop has " << numBBs << " basic blocks.\n";

      // Return 'all' if no transformation is done and everything is preserved.
      return PreservedAnalyses::all();
    }
  };
} // end anonymous namespace

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------

// This is the core interface for pass plugins. It tells LLVM how to register 
// and create your pass within the new pass manager pipeline.
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "MySimpleLoopPass", "v0.1",
    [](PassBuilder &PB) {
      // Register our loop pass
      PB.registerPipelineParsingCallback(
          [&](StringRef Name, LoopPassManager &LPM,
              ArrayRef<PassBuilder::PipelineElement>) {
            if (Name == "my-simple-loop-pass") {
              LPM.addPass(MySimpleLoopPass());
              return true;
            }
            return false;
          }
      );
    }
  };
}
