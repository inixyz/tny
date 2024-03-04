#include <list>
#include <string>
#include <variant>

#include "lex.hpp"
#include "err.hpp"
#include <iostream>

enum Operator {
    ADD, MUL
};

struct Expr {
    enum {
        INT, FLOAT,
        INFIX,
    } type;

    struct Infix {
        Operator op;
        Expr *left, *right;
    };

    std::variant<int, float, std::string, Infix> val;
};

enum Precedence {
    LOWEST,
    SUM,
    PRODUCT,
};

struct LetStatement {
    std::string identifier;
    Expr* expr;
};

struct ReturnStatement {
    Expr* expr;
};

typedef std::variant<LetStatement> Statement;

class Parser {
public:
    Parser(const std::list<Tok> toks) {
        this->toks = toks;
    }

////////////////////////////////////////////////////////////////////////////////

std::list<Statement> parse_program() {
    std::list<Statement> program;

    while (toks.front().type != Tok::END) {
        program.push_back(parse_statement());
    }

    return program;
}

////////////////////////////////////////////////////////////////////////////////


private:
    std::list<Tok> toks;

////////////////////////////////////////////////////////////////////////////////

Precedence get_tok_precedence() {
    switch (toks.front().type) {
    case Tok::ADD: return SUM;
    case Tok::MUL: return PRODUCT;
    default: break;
    }
}

Operator get_tok_operator() {
    switch (toks.front().type) {
    case Tok::ADD: return Operator::ADD;
    case Tok::MUL: return Operator::MUL;
    default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////

Expr* parse_int_expr() {
    Expr* expr = new Expr;
    expr->type = Expr::INT;
    expr->val = std::get<int>(toks.front().literal);
    toks.pop_front();

    return expr;
}


////////////////////////////////////////////////////////////////////////////////

Expr* parse_infix_expr(Expr* left) {
    Expr* expr = new Expr;
    expr->type = Expr::INFIX;

    Expr::Infix infix;

    infix.left = left;

    infix.op = get_tok_operator();
    Precedence precedence = get_tok_precedence();

    toks.pop_front();

    infix.right = parse_expression(precedence);

    expr->val = infix;

    return expr;
}

////////////////////////////////////////////////////////////////////////////////

Expr* parse_expression(Precedence precedence) {
    Expr* expr = NULL;

    switch(toks.front().type) {
    case Tok::INT: expr = parse_int_expr(); break;
    default: break;
    }

    while (toks.front().type != Tok::SEMICOLON && precedence < get_tok_precedence()) {
        switch (toks.front().type) {
        case Tok::ADD: case Tok::MUL: expr = parse_infix_expr(expr);
        default: break;
        }
    }

    return expr;
}

////////////////////////////////////////////////////////////////////////////////

Statement parse_let_statement() {
    toks.pop_front(); // remove let from start

    if (toks.front().type != Tok::IDENT) err::fatal("no ident after let");
    std::string identifier = std::get<std::string>(toks.front().literal);
    toks.pop_front();

    if (toks.front().type != Tok::ASSIGN) err::fatal("no assign in let statement");
    toks.pop_front();

    Expr* expr = parse_expression(LOWEST);

    if (toks.front().type != Tok::SEMICOLON) err::fatal("no semicolon after let statement");
    toks.pop_front();

    return (LetStatement){identifier, expr};
}

////////////////////////////////////////////////////////////////////////////////

Statement parse_statement() {
    switch (toks.front().type) {
    case Tok::LET: return parse_let_statement();
    default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////
};

void print_expr(Expr* expr, std::string whitespace);

void print_infix(Expr::Infix infix, std::string whitespace) {
    std::cout << whitespace << "op=" << infix.op << std::endl;

    std::cout << whitespace << "left=" << std::endl;
    print_expr(infix.left, whitespace + "\t|");

    std::cout << whitespace << "right=" << std::endl;
    print_expr(infix.right, whitespace + "\t|");
}

void print_expr(Expr* expr, std::string whitespace) {
    switch (expr->type) {
    case Expr::INT: std::cout << whitespace << std::get<int>(expr->val) << std::endl; break;
    case Expr::INFIX: print_infix(std::get<Expr::Infix>(expr->val), whitespace); break;
    }
}

void print_let(const LetStatement& stmt) {
    std::cout << "identifier=" << stmt.identifier << std::endl;
    std::cout << "expr=" << std::endl;
    print_expr(stmt.expr, "\t|");
    std::cout << std::endl;
}

int main() {
    std::list<Tok> toks = Lex("let str = 5 + 6 * 3 + 7;").get_toks();
    std::list<Statement> program = Parser(toks).parse_program();

    for (auto it = program.begin(); it != program.end(); it++) {
        LetStatement& stmt = std::get<LetStatement>(*it);
        print_let(stmt);
    }
}

// let statement:  tok_let tok_ident tok_assign expersion tok_semicolon
// return statement tok_ret experession
// expresion statement


/*
let get_sum_func = func() {
    return func(a, b) {
        return a + b;
    }
};

let sum = get_sum_func();

let statement
return statement
func statement

if statement
while statement
for statement

expr se evalueaza intr o valoare

if () {

}

statement does something


if () {

}


sum(a, b)
*/