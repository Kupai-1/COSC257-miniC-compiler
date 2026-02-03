%{
#include "ast/ast.h"
#include "y.tab.h"
#include <string.h>
%}

%%

"extern"     { return EXTERN; }
"void"       { return VOID; }
"int"        { return INT; }
"if"         { return IF; }
"else"       { return ELSE; }
"while"      { return WHILE; }
"return"     { return RETURN; }
"print"      { return PRINT; }
"read"       { return READ; }

[a-z]+       { 
    yylval.str = strdup(yytext); 
    return ID; 
}

[0-9]+       { 
    yylval.num = atoi(yytext); 
    return NUM; 
}

"<="         { return LE; }
">="         { return GE; }
"=="         { return EQ; }
"!="         { return NE; }

[-+*/()<>=;{},] { return yytext[0]; }

[ \t\n]+     { /* skip whitespace */ }

.            { 
    fprintf(stderr, "unexpected character: %s\n", yytext); 
}

%%

int yywrap() { 
    return 1; 
}
