#pragma once

#include <variant>
#include <string>
#include <list>

#include "lex.hpp"

struct Expr {
    enum {
        INT, FLOAT, INFIX
    } type;

    struct Infix {
        enum {
            SUM, SUB
        } operation;
        Expr *left, *right;
    };

    std::variant<int, float, Infix> data;
};

struct Stmt {
    enum {
        LET, RET
    } type;

    struct Let {
        std::string ident;
        Expr* expr;
    };

    std::variant<Let> data;
};

class Parser {
public:
    Parser(const std::list<Tok> toks);
    std::list<Stmt> get_stmts();

private:
    std::list<Tok> toks;

    Stmt parse_stmt();
    Stmt parse_let();
    Expr* parse_expr(const int precedence);
};