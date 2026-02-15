#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ============================================================================
// HELPER FUNCTION: is_safe_to_delete
// ============================================================================
// returns true if instruction can be deleted when it has no uses
// returns false for instructions with side effects (store, call, etc.)
bool is_safe_to_delete(LLVMValueRef inst) {
    LLVMOpcode opcode = LLVMGetInstructionOpcode(inst);
    
    // don't delte instructions that are terminators (return, branch, etc.) or have side effects (store, call, etc.)
    switch(opcode) {
        // terminator instructions 
        case LLVMRet:           // return statement
        case LLVMBr:            // branch
        case LLVMSwitch:        // switch statement
        case LLVMIndirectBr:    // indirect branch
        case LLVMInvoke:        // exception handling
        case LLVMUnreachable:   // unreachable code marker
            return false;
        
        // memory/side effect instructions
        case LLVMStore:         // writes to memory (side effect!)
        case LLVMCall:          // function call (might have side effects)
        case LLVMAlloca:        // memory allocation
        case LLVMFence:         // memory fence
        case LLVMAtomicCmpXchg: // atomic operations
        case LLVMAtomicRMW:     // atomic read-modify-write
            return false;
        
        // everything else is safe to delete
        default:
            return true;
    }
}

// ============================================================================
// OPTIMIZATION 1: DEAD CODE ELIMINATION
// ============================================================================
// removes instructions that have no uses and are safe to delete
// example: %7 = load i32, ptr %3   <- if %7 is never used, delete it
bool dead_code_elimination(LLVMModuleRef module) {
    bool changed = false;
    
    // iterate through all functions in the module
    for (LLVMValueRef function = LLVMGetFirstFunction(module);
         function != NULL;
         function = LLVMGetNextFunction(function)) {
        
        // iterate through all basic blocks in the function
        for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(function);
             bb != NULL;
             bb = LLVMGetNextBasicBlock(bb)) {
            
            // iterate through instructions in the basic block
            // IMPORTANT: save next instruction before deleting current one!
            LLVMValueRef inst = LLVMGetFirstInstruction(bb);
            while (inst != NULL) {
                LLVMValueRef next_inst = LLVMGetNextInstruction(inst);
                
                // check if instruction has zero uses AND is safe to delete
                if (LLVMGetFirstUse(inst) == NULL && is_safe_to_delete(inst)) {
                    LLVMInstructionEraseFromParent(inst);
                    changed = true;
                }
                
                inst = next_inst;
            }
        }
    }
    
    return changed;
}

// ============================================================================
// OPTIMIZATION 2: CONSTANT FOLDING
// ============================================================================
// pre-computes arithmetic on constants at compile time
// example: %9 = add i32 10, 20  ->  replace %9 with constant 30
bool constant_folding(LLVMModuleRef module) {
    bool changed = false;
    
    // iterate through all functions
    for (LLVMValueRef function = LLVMGetFirstFunction(module);
         function != NULL;
         function = LLVMGetNextFunction(function)) {
        
        // iterate through all basic blocks
        for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(function);
             bb != NULL;
             bb = LLVMGetNextBasicBlock(bb)) {
            
            // iterate through instructions
            LLVMValueRef inst = LLVMGetFirstInstruction(bb);
            while (inst != NULL) {
                LLVMValueRef next_inst = LLVMGetNextInstruction(inst);
                
                LLVMOpcode opcode = LLVMGetInstructionOpcode(inst);
                
                // check if this is an arithmetic operation (+, -, *)
                if (opcode == LLVMAdd || opcode == LLVMSub || opcode == LLVMMul) {
                    // get the two operands
                    LLVMValueRef op1 = LLVMGetOperand(inst, 0);
                    LLVMValueRef op2 = LLVMGetOperand(inst, 1);
                    
                    // check if BOTH operands are constants
                    if (LLVMIsConstant(op1) && LLVMIsConstant(op2)) {
                        LLVMValueRef const_result = NULL;
                        
                        // compute the result based on the operation
                        switch(opcode) {
                            case LLVMAdd:
                                const_result = LLVMConstAdd(op1, op2);
                                break;
                            case LLVMSub:
                                const_result = LLVMConstSub(op1, op2);
                                break;
                            case LLVMMul:
                                const_result = LLVMConstMul(op1, op2);
                                break;
                            default:
                                break;
                        }
                        
                        // if we successfully computed a constant
                        if (const_result != NULL) {
                            // replace all uses of the instruction with the constant
                            // example: if %9 = add 10, 20, replace all uses of %9 with 30
                            LLVMReplaceAllUsesWith(inst, const_result);
                            changed = true;
                        }
                    }
                }
                
                inst = next_inst;
            }
        }
    }
    
    return changed;
}

// ============================================================================
// HELPER FUNCTION: instructions_equal
// ============================================================================
// checks if two instructions are equivalent (same opcode and operands)
// example: %1 = add %a, %b  and  %3 = add %a, %b  are equal
bool instructions_equal(LLVMValueRef inst1, LLVMValueRef inst2) {
    // check if opcodes match (both add, both mul, etc.)
    if (LLVMGetInstructionOpcode(inst1) != LLVMGetInstructionOpcode(inst2)) {
        return false;
    }
    
    // check if they have the same number of operands
    int num_ops = LLVMGetNumOperands(inst1);
    if (num_ops != LLVMGetNumOperands(inst2)) {
        return false;
    }
    
    // check if ALL operands are identical
    for (int i = 0; i < num_ops; i++) {
        if (LLVMGetOperand(inst1, i) != LLVMGetOperand(inst2, i)) {
            return false;
        }
    }
    
    return true;
}

// ============================================================================
// OPTIMIZATION 3: COMMON SUBEXPRESSION ELIMINATION
// ============================================================================
// removes duplicate calculations within a basic block
// example: %1 = add %a, %b
//          %2 = mul %1, 5
//          %3 = add %a, %b    <- duplicate! replace with %1
bool common_subexpression_elimination(LLVMModuleRef module) {
    bool changed = false;
    
    // iterate through all functions
    for (LLVMValueRef function = LLVMGetFirstFunction(module);
         function != NULL;
         function = LLVMGetNextFunction(function)) {
        
        // iterate through all basic blocks
        for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(function);
             bb != NULL;
             bb = LLVMGetNextBasicBlock(bb)) {
            
            // for each instruction A in the basic block
            for (LLVMValueRef inst_a = LLVMGetFirstInstruction(bb);
                 inst_a != NULL;
                 inst_a = LLVMGetNextInstruction(inst_a)) {
                
                LLVMOpcode opcode_a = LLVMGetInstructionOpcode(inst_a);
                
                // skip instructions we should NEVER eliminate
                if (opcode_a == LLVMCall ||      // function calls
                    opcode_a == LLVMStore ||     // memory writes
                    opcode_a == LLVMAlloca ||    // memory allocation
                    LLVMIsATerminatorInst(inst_a)) {  // control flow
                    continue;
                }
                
                // look for instruction B that comes after A
                for (LLVMValueRef inst_b = LLVMGetNextInstruction(inst_a);
                     inst_b != NULL;
                     inst_b = LLVMGetNextInstruction(inst_b)) {
                    
                    // check if A and B are equivalent
                    if (instructions_equal(inst_a, inst_b)) {
                        
                        // SPECIAL CASE: if both are load instructions
                        // need to check if a store happened between them
                        if (LLVMGetInstructionOpcode(inst_a) == LLVMLoad) {
                            bool safe = true;
                            LLVMValueRef load_addr = LLVMGetOperand(inst_a, 0);
                            
                            // check all instructions between A and B
                            for (LLVMValueRef between = LLVMGetNextInstruction(inst_a);
                                 between != inst_b && between != NULL;
                                 between = LLVMGetNextInstruction(between)) {
                                
                                // if there's a store to the same address, not safe!
                                if (LLVMGetInstructionOpcode(between) == LLVMStore) {
                                    LLVMValueRef store_addr = LLVMGetOperand(between, 1);
                                    if (store_addr == load_addr) {
                                        safe = false;
                                        break;
                                    }
                                }
                            }
                            
                            // if not safe, skip this elimination
                            if (!safe) {
                                continue;
                            }
                        }
                        
                        // safe to eliminate: replace all uses of B with A
                        LLVMReplaceAllUsesWith(inst_b, inst_a);
                        changed = true;
                    }
                }
            }
        }
    }
    
    return changed;
}

// ============================================================================
// GLOBAL OPTIMIZATION 
// ============================================================================
bool constant_propagation(LLVMModuleRef module) {
    return false;
}

// ============================================================================
// HELPER FUNCTION: is_constant_store (for constant propagation)
// ============================================================================
bool is_constant_store(LLVMValueRef inst) {
    return false;
}