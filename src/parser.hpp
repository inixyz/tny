#pragma once

// C++ headers
#include <variant>
#include <string>
#include <list>

// internal headers
#include "lexer.hpp"

struct Expr {
    enum {
        FLOAT, INFIX
    } type;

    struct Infix {
        enum {
            SUM, SUB
        } operation;
        Expr *left, *right;
    };

    std::variant<float, std::string, Infix> data;
};

struct Stmt {
    enum {
        LET
    } type;

    struct Let {
        std::string ident; Expr* expr;
    };

    std::variant<Let> data;
};

class Parser {
public:
    Parser(const std::list<Token> tokens);
    std::list<Stmt> get_stmts();

private:
    std::list<Token> tokens;

    Stmt parse_stmt();
    Stmt parse_let();
    Expr* parse_expr(const int precedence);
};