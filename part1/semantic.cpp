#include "ast/ast.h"
#include <stack>
#include <set>
#include <string>
#include <cstdio>

using namespace std;

extern "C" {
    int check_semantics(astNode *root);
}

class SemanticChecker {
private:
    stack<set<string>> scopes;
    bool has_error;
    
    void enter_scope() {
        scopes.push(set<string>());
    }
    
    void exit_scope() {
        scopes.pop();
    }
    
    bool declare(const char *name) {
        if (scopes.top().find(name) != scopes.top().end()) {
            fprintf(stderr, "semantic error: duplicate declaration of '%s'\n", name);
            has_error = true;
            return false;
        }
        scopes.top().insert(name);
        return true;
    }
    
    bool check_declared(const char *name) {
        stack<set<string>> temp = scopes;
        while (!temp.empty()) {
            if (temp.top().find(name) != temp.top().end()) {
                return true;
            }
            temp.pop();
        }
        fprintf(stderr, "semantic error: '%s' used before declaration\n", name);
        has_error = true;
        return false;
    }
    
    void visit_prog(astNode *node) {
        visit_func(node->prog.func);
    }
    
    void visit_func(astNode *node) {
        enter_scope();
        
        if (node->func.param != NULL) {
            declare(node->func.param->var.name);
        }
        
        // visit function body - pass flag to indicate it's the top-level function block
        visit_stmt(node->func.body, true);
        exit_scope();
    }
    
    void visit_stmt(astNode *node, bool is_func_body = false) {
        if (node == NULL) return;
        
        switch (node->stmt.type) {
            case ast_call:
                if (node->stmt.call.param != NULL) {
                    visit_expr(node->stmt.call.param);
                }
                break;
                
            case ast_ret:
                visit_expr(node->stmt.ret.expr);
                break;
                
            case ast_block: {
                // only create new scope for nested blocks, not function body
                if (!is_func_body) {
                    enter_scope();
                }
                
                vector<astNode*> *slist = node->stmt.block.stmt_list;
                for (auto it = slist->begin(); it != slist->end(); ++it) {
                    visit_node(*it);
                }
                
                if (!is_func_body) {
                    exit_scope();
                }
                break;
            }
                
            case ast_while:
                visit_expr(node->stmt.whilen.cond);
                visit_stmt(node->stmt.whilen.body);
                break;
                
            case ast_if:
                visit_expr(node->stmt.ifn.cond);
                visit_stmt(node->stmt.ifn.if_body);
                if (node->stmt.ifn.else_body != NULL) {
                    visit_stmt(node->stmt.ifn.else_body);
                }
                break;
                
            case ast_decl:
                declare(node->stmt.decl.name);
                break;
                
            case ast_asgn:
                check_declared(node->stmt.asgn.lhs->var.name);
                visit_expr(node->stmt.asgn.rhs);
                break;
        }
    }
    
    void visit_expr(astNode *node) {
        if (node == NULL) return;
        
        switch (node->type) {
            case ast_var:
                check_declared(node->var.name);
                break;
                
            case ast_cnst:
                break;
                
            case ast_rexpr:
                visit_expr(node->rexpr.lhs);
                visit_expr(node->rexpr.rhs);
                break;
                
            case ast_bexpr:
                visit_expr(node->bexpr.lhs);
                visit_expr(node->bexpr.rhs);
                break;
                
            case ast_uexpr:
                visit_expr(node->uexpr.expr);
                break;
                
            case ast_stmt:
                visit_stmt(node);
                break;
                
            case ast_prog:
            case ast_func:
            case ast_extern:
                break;
        }
    }
    
    void visit_node(astNode *node) {
        if (node == NULL) return;
        
        switch (node->type) {
            case ast_prog:
                visit_prog(node);
                break;
            case ast_func:
                visit_func(node);
                break;
            case ast_stmt:
                visit_stmt(node);
                break;
            default:
                visit_expr(node);
                break;
        }
    }
    
public:
    int check(astNode *root) {
        has_error = false;
        visit_node(root);
        return has_error ? 1 : 0;
    }
};

int check_semantics(astNode *root) {
    SemanticChecker checker;
    return checker.check(root);
}