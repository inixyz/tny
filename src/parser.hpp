#pragma once

// C++ headers
#include <variant>
#include <string>
#include <list>

// internal headers
#include "lexer.hpp"

struct Expr {
    enum Type {
        IDENT, FLOAT, STR, PREFIX, IF, INFIX
    } type;

    struct Prefix {
        enum Operation {
            NEG, NOT
        } operation;
        Expr* right;
    };

    struct If {
        Expr* condition;
    };

    struct Infix {
        enum Operation {
            SUM, SUB, MUL, DIV, MOD, EQ, NEQ, GT, LT, GEQ, LEQ, AND, OR
        } operation;
        Expr *left, *right;
    };

    std::variant<float, std::string, Prefix, If, Infix> data;
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
    Expr* parse_expr_val(const Expr::Type type);
    Expr* parse_expr_prefix();
    Expr* parse_expr_group();
    Expr* parse_expr_if();
    Expr* parse_expr_infix(Expr* expr);

    const std::unordered_map<Token::Type, int> PRECEDENCES = {
        {Token::ASSIGN, 1},
        {Token::OR, 2},
        {Token::AND, 3},
        {Token::EQ, 4}, {Token::NEQ, 4},
        {Token::LT, 5}, {Token::LEQ, 5}, {Token::GT, 5}, {Token::GEQ, 5},
        {Token::PLUS, 6}, {Token::MINUS, 6},
        {Token::ASTERISK, 7}, {Token::SLASH, 7}, {Token::PERCENT, 7},
        // PREFIX 8
        {Token::LPAREN, 9},
        {Token::LBRACKET, 10}
    };

    const std::unordered_map<Token::Type, Expr::Infix::Operation> OPERATIONS = {
        {Token::PLUS, Expr::Infix::SUM}, {Token::MINUS, Expr::Infix::SUB},
        {Token::ASTERISK, Expr::Infix::MUL}, {Token::SLASH, Expr::Infix::DIV},
        {Token::PERCENT, Expr::Infix::MOD}, {Token::EQ, Expr::Infix::EQ},
        {Token::NEQ, Expr::Infix::NEQ}, {Token::GT, Expr::Infix::GT},
        {Token::LT, Expr::Infix::LT}, {Token::GEQ, Expr::Infix::GEQ},
        {Token::LEQ, Expr::Infix::LEQ}, {Token::AND, Expr::Infix::AND},
        {Token::OR, Expr::Infix::OR}
    };
};