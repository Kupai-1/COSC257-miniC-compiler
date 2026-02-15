#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    // check command line arguments
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input.ll>\n", argv[0]);
        fprintf(stderr, "example: %s optimizer_test_results/cfold_add.ll\n", argv[0]);
        return 1;
    }
    
    // ========================================================================
    // STEP 1: load the LLVM IR file
    // ========================================================================
    
    LLVMMemoryBufferRef buffer;
    char *error_msg = NULL;
    
    // create memory buffer from file
    if (LLVMCreateMemoryBufferWithContentsOfFile(argv[1], &buffer, &error_msg)) {
        fprintf(stderr, "error loading file '%s': %s\n", argv[1], error_msg);
        LLVMDisposeMessage(error_msg);
        return 1;
    }
    
    // ========================================================================
    // STEP 2: parse IR into a module
    // ========================================================================
    
    LLVMModuleRef module;
    if (LLVMParseIRInContext(LLVMGetGlobalContext(), buffer, &module, &error_msg)) {
        fprintf(stderr, "error parsing IR: %s\n", error_msg);
        LLVMDisposeMessage(error_msg);
        LLVMDisposeMemoryBuffer(buffer);
        return 1;
    }
    
    // ========================================================================
    // STEP 3: run optimizations in a loop until fixed point
    // ========================================================================
    // we keep running optimizations until nothing changes
    
    bool changed = true;
    int iteration = 0;
    
    while (changed) {
        changed = false;
        iteration++;
        
        // run dead code elimination
        // removes instructions with no uses
        bool dce_changed = dead_code_elimination(module);
        changed |= dce_changed;
        
        // run constant folding
        // pre-computes arithmetic on constants
        bool cf_changed = constant_folding(module);
        changed |= cf_changed;
        
        // run common subexpression elimination
        // removes duplicate calculations
        bool cse_changed = common_subexpression_elimination(module);
        changed |= cse_changed;
        
        // run constant propagation (when implemented)
        // tracks constants through store/load instructions
        bool cp_changed = constant_propagation(module);
        changed |= cp_changed;
    }
    
    // ========================================================================
    // STEP 4: output the optimized IR to stdout
    // ========================================================================
    
    // convert module to string
    char *ir_string = LLVMPrintModuleToString(module);
    
    // print to stdout (so we can redirect to file)
    printf("%s", ir_string);
    
    // cleanup
    LLVMDisposeMessage(ir_string);
    
    // ========================================================================
    // STEP 5: cleanup and exit
    // ========================================================================
    
    LLVMDisposeModule(module);
    
    return 0;
}