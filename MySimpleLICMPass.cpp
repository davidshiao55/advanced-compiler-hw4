//===- MySimpleLICMPass.cpp - A Simple LICM Pass (New PM) ---------*- C++ -*-===//
//
// This is a toy example of Loop-Invariant Code Motion using the new pass manager.
//
//===----------------------------------------------------------------------===//

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

// A helper to check if an instruction is safe to hoist (simplified).
static bool isLoopInvariantInstruction(Instruction &I, Loop *L)
{
    // 0) Never hoist terminators
    if (I.isTerminator()) {
        return false;
    }

    // 1) If it may have side effects, skip
    if (I.mayHaveSideEffects()) {
        return false;
    }

    // 2) Check if all operands are defined outside the loop (simplified)
    for (Use &U : I.operands()) {
        auto *OpInst = dyn_cast<Instruction>(U.get());
        if (OpInst && L->contains(OpInst))
            return false;
    }

    return true;
}

namespace
{
class MySimpleLICMPass : public PassInfoMixin<MySimpleLICMPass>
{
    bool hasLoopInvariantOperands(const Loop &L, const DenseSet<Value *> &LI, const Instruction &Inst)
    {
        for (const auto &Use : Inst.operands()) {
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
        DominatorTree &DT = AR.DT;

        // If the loop doesn't have a single preheader block, it's harder to do LICM.
        // In real LICM, we handle multiple preds or zero preds carefully.
        BasicBlock *Preheader = L.getLoopPreheader();
        if (!Preheader) {
            errs() << "[MySimpleLICM] No single loop preheader, skipping.\n";
            return PreservedAnalyses::all();
        }

        bool Changed = false;
        bool Converged = false;

        // We'll store instructions that are safe to hoist in a small vector
        // and then move them out of the loop.
        SmallVector<Instruction *, 8> ToHoist;
        DenseSet<Value *> LI;

        while (!Converged) {
            Converged = true;
            // For each block in the loop
            for (BasicBlock *BB : L.blocks()) {
                // Don't consider the loop header itself if it's also the preheader
                // (rare, but can happen in certain degenerate CFGs).
                if (BB == Preheader)
                    continue;

                // Iterate over all instructions in the block
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
                }
            }
        }

        // Move them to the end of the preheader
        for (Instruction *I : ToHoist) {
            I->moveBefore(Preheader->getTerminator());
            Changed = true;
            errs() << "[MySimpleLICM] Hoisted: " << *I << "\n";
        }

        if (Changed) {
            // We modified the IR, so let LLVM know we potentially changed analyses.
            // Because we might affect dominance, alias analysis, etc., we invalidate
            // some of them. For a real pass, you'd be more precise in what you preserve.
            return PreservedAnalyses::none();
        } else {
            return PreservedAnalyses::all();
        }
    }
};
} // end anonymous namespace

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------

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
