%{
#include <stdio.h>
#include <stdlib.h>
#include "ast/ast.h"

extern int yylex();
extern FILE *yyin;
int yyerror(const char *s);

astNode *ast_root = NULL;
%}

%union {
    int num;
    char *str;
    astNode *node;
    vector<astNode*> *stmt_list;
}

%token <num> NUM
%token <str> ID
%token EXTERN VOID INT IF ELSE WHILE RETURN PRINT READ
%token LE GE EQ NE

%type <node> program extern_decl function param
%type <node> stmt expr decl
%type <stmt_list> decl_list stmt_list

%right '='
%nonassoc IFX
%nonassoc ELSE
%nonassoc '<' '>' LE GE EQ NE
%left '+' '-'
%left '*' '/'
%right UMINUS

%start program

%%

program : extern_decl extern_decl function 
        { 
            $$ = createProg($1, $2, $3);
            ast_root = $$;
        }
        ;

extern_decl : EXTERN VOID PRINT '(' INT ')' ';'
            { $$ = createExtern("print"); }
            | EXTERN INT READ '(' ')' ';'
            { $$ = createExtern("read"); }
            ;

function : INT ID '(' ')' '{' decl_list stmt_list '}'
         {
             for (auto it = $6->rbegin(); it != $6->rend(); ++it) {
                 $7->insert($7->begin(), *it);
             }
             delete $6;
             astNode *body = createBlock($7);
             $$ = createFunc($2, NULL, body);
         }
         | INT ID '(' param ')' '{' decl_list stmt_list '}'
         {
             for (auto it = $7->rbegin(); it != $7->rend(); ++it) {
                 $8->insert($8->begin(), *it);
             }
             delete $7;
             astNode *body = createBlock($8);
             $$ = createFunc($2, $4, body);
         }
         ;

param : INT ID
      { $$ = createVar($2); }
      ;

decl_list : decl_list decl
          { 
              $$ = $1;
              $$->push_back($2);
          }
          | /* empty */
          { $$ = new vector<astNode*>(); }
          ;

decl : INT ID ';'
     { $$ = createDecl($2); }
     ;

stmt_list : stmt_list stmt
          {
              $$ = $1;
              $$->push_back($2);
          }
          | stmt
          { 
              $$ = new vector<astNode*>();
              $$->push_back($1);
          }
          ;

stmt : ID '=' expr ';'
     { $$ = createAsgn(createVar($1), $3); }
     | PRINT '(' expr ')' ';'
     { $$ = createCall("print", $3); }
     | RETURN expr ';'
     { $$ = createRet($2); }
     | WHILE '(' expr ')' stmt
     { $$ = createWhile($3, $5); }
     | IF '(' expr ')' stmt %prec IFX
     { $$ = createIf($3, $5); }
     | IF '(' expr ')' stmt ELSE stmt
     { $$ = createIf($3, $5, $7); }
     | '{' decl_list stmt_list '}'
     {
         for (auto it = $2->rbegin(); it != $2->rend(); ++it) {
             $3->insert($3->begin(), *it);
         }
         delete $2;
         $$ = createBlock($3);
     }
     ;

expr : expr '<' expr
     { $$ = createRExpr($1, $3, lt); }
     | expr '>' expr
     { $$ = createRExpr($1, $3, gt); }
     | expr LE expr
     { $$ = createRExpr($1, $3, le); }
     | expr GE expr
     { $$ = createRExpr($1, $3, ge); }
     | expr EQ expr
     { $$ = createRExpr($1, $3, eq); }
     | expr NE expr
     { $$ = createRExpr($1, $3, neq); }
     | expr '+' expr
     { $$ = createBExpr($1, $3, add); }
     | expr '-' expr
     { $$ = createBExpr($1, $3, sub); }
     | expr '*' expr
     { $$ = createBExpr($1, $3, mul); }
     | expr '/' expr
     { $$ = createBExpr($1, $3, divide); }
     | '-' expr %prec UMINUS
     { $$ = createUExpr($2, uminus); }
     | '(' expr ')'
     { $$ = $2; }
     | ID
     { $$ = createVar($1); }
     | NUM
     { $$ = createCnst($1); }
     | READ '(' ')'
     { $$ = createCall("read"); }
     ;

%%

int yyerror(const char *s) {
    fprintf(stderr, "parse error: %s\n", s);
    return 0;
}
