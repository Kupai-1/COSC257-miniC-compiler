#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>

// ============================================================================
// LOCAL OPTIMIZATIONS
// ============================================================================

// dead code elimination: removes instructions with no uses
bool dead_code_elimination(LLVMModuleRef module);

// constant folding: pre-computes arithmetic on constants (10 + 20 -> 30)
bool constant_folding(LLVMModuleRef module);

// common subexpression elimination: removes duplicate calculations
bool common_subexpression_elimination(LLVMModuleRef module);

// ============================================================================
// GLOBAL OPTIMIZATION 
// ============================================================================

// constant propagation: tracks constants through store/load instructions
bool constant_propagation(LLVMModuleRef module);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// check if instruction can be safely deleted (preserves side effects)
bool is_safe_to_delete(LLVMValueRef inst);

// check if two instructions are equivalent (same opcode and operands)
bool instructions_equal(LLVMValueRef inst1, LLVMValueRef inst2);

// check if a store instruction stores a constant value
bool is_constant_store(LLVMValueRef inst);

#endif