#include <stdio.h>
#include "ast/ast.h"

extern int yyparse();
extern FILE *yyin;
extern int yylex_destroy();
extern astNode *ast_root;

extern "C" {
    int check_semantics(astNode *root);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        fprintf(stderr, "cannot open file: %s\n", argv[1]);
        return 1;
    }
    
    printf("parsing %s...\n", argv[1]);
    if (yyparse() != 0) {
        fprintf(stderr, "parse failed\n");
        fclose(yyin);
        return 1;
    }
    printf("parse successful\n\n");
    
    printf("AST:\n");
    printNode(ast_root);
    printf("\n");
    
    printf("checking semantics...\n");
    int result = check_semantics(ast_root);
    
    if (result == 0) {
        printf("semantic check passed\n");
    } else {
        printf("semantic check failed\n");
    }
    
    fclose(yyin);
    yylex_destroy();
    freeNode(ast_root);
    
    return result;
}