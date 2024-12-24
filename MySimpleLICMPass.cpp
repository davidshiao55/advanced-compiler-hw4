#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace
{
class MySimpleLICMPass : public PassInfoMixin<MySimpleLICMPass>
{
    bool hasLoopInvariantOperands(const Loop &L, const DenseSet<Value *> &LI, const Instruction &I)
    {
        for (const auto &Use : I.operands()) {
            if (LI.contains(&*Use))
                // Operand already marked LI
                continue;

            if (auto *UseInst = dyn_cast<Instruction>(&*Use)) {
                if (L.contains(UseInst)) {
                    // The def of this operand is inside the loop
                    return false;
                }
            }
        }
        return true;
    }

  public:
    PreservedAnalyses run(Loop &L, LoopAnalysisManager &LAM,
                          LoopStandardAnalysisResults &AR, LPMUpdater &Updater)
    {

        // Without a preheader, hoisting is not feasible.
        BasicBlock *Preheader = L.getLoopPreheader();
        if (!Preheader) {
            errs() << "[MySimpleLICM] No single loop preheader, skipping.\n";
            return PreservedAnalyses::all();
        }

        bool Changed = false;
        bool Converged = false;

        // Hoist Set
        SmallVector<Instruction *, 8> ToHoist;
        // Loop Invariant Set
        DenseSet<Value *> LI;

        while (!Converged) {
            Converged = true;
            for (BasicBlock *BB : L.blocks()) {
                // Don't consider the loop header itself if it's also the preheader
                if (BB == Preheader)
                    continue;

                for (Instruction &Inst : *BB) {
                    if (LI.contains(&Inst) ||                   // Already have marked LI
                        !isSafeToSpeculativelyExecute(&Inst) || // Cannot hoist effectful instruction
                        Inst.mayReadOrWriteMemory() ||          // Conservatively, don't hoist any memory ops
                        !hasLoopInvariantOperands(L, LI, Inst))
                        continue; // Operands not LI;

                    // Otherwise, we mark this instruction as LI
                    LI.insert(&Inst);
                    ToHoist.push_back(&Inst);
                    Converged = false;
                    Changed = true;
                }
            }
        }

        // Move them to the end of the preheader
        for (Instruction *I : ToHoist) {
            I->moveBefore(Preheader->getTerminator());
            Changed = true;
            errs() << "[MySimpleLICM] Hoisted: " << *I << "\n";
        }

        return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
    }
};
} // end anonymous namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
    return {
        LLVM_PLUGIN_API_VERSION, "MySimpleLICMPass", "v0.1",
        [](PassBuilder &PB) {
            // Register our loop pass
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, LoopPassManager &LPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "my-simple-licm-pass") {
                        LPM.addPass(MySimpleLICMPass());
                        return true;
                    }
                    return false;
                });
        }};
}
