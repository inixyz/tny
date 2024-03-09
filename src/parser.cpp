#include "parser.hpp"

// C++ headers
#include <list>
#include <string>

// internal headers
#include "lexer.hpp"
#include "err.hpp"

////////////////////////////////////////////////////////////////////////////////

Parser::Parser(const std::list<Token> tokens) {
    this->tokens = tokens;
}

////////////////////////////////////////////////////////////////////////////////

std::list<Stmt> Parser::get_stmts() {
    std::list<Stmt> stmts;
    while (tokens.front().type != Token::END) stmts.push_back(parse_stmt());
    return stmts;
}

////////////////////////////////////////////////////////////////////////////////

Stmt Parser::parse_stmt() {
    switch (tokens.front().type) {
    case Token::LET: return parse_let();
    }
}

////////////////////////////////////////////////////////////////////////////////

Stmt Parser::parse_let() {
    tokens.pop_front();

    if (tokens.front().type != Token::IDENT)
        err::fatal("Missing identifier after 'let' keyword");

    const std::string ident = std::get<std::string>(tokens.front().literal);
    tokens.pop_front();

    if (tokens.front().type != Token::ASSIGN)
        err::fatal("Expected '=' after '" + ident + "'");

    Expr* expr = parse_expr(-1);

    if (tokens.front().type != Token::SEMICOLON)
        err::fatal("Expected ';' after expression");

    return {Stmt::LET, Stmt::Let{ident, expr}};
}

////////////////////////////////////////////////////////////////////////////////

Expr* Parser::parse_expr(const int precedence) {
    Expr* expr;

    switch (tokens.front().type) {
    case Token::IDENT: expr = parse_expr_val(Expr::IDENT); break;
    case Token::FLOAT: expr = parse_expr_val(Expr::FLOAT); break;
    case Token::STR: expr = parse_expr_val(Expr::STR); break;
    case Token::MINUS: case Token::NOT:
        expr = parse_expr_prefix(); break;
    case Token::LPAREN: expr = parse_expr_group(); break;
    case Token::IF: expr = parse_expr_if(); break;

    default: err::fatal("Invalid token in expression");
    }

    while (tokens.front().type != Token::SEMICOLON
        && precedence < PRECEDENCES.at(tokens.front().type)) {

        switch (tokens.front().type) {
        case Token::PLUS ... Token::OR: expr = parse_expr_infix(expr); break;
        default: return expr;
        }
    }

    return expr;
}

////////////////////////////////////////////////////////////////////////////////

Expr* Parser::parse_expr_val(const Expr::Type type) {
    Expr* expr = new Expr;
    expr->type = type;

    switch (type) {
    case Expr::IDENT: case Expr::STR:
        expr->data = std::get<std::string>(tokens.front().literal);
        break;
    case Expr::FLOAT:
        expr->data = std::get<float>(tokens.front().literal);
        break;
    }

    return expr;
}

////////////////////////////////////////////////////////////////////////////////

Expr* Parser::parse_expr_prefix() {
    Expr* expr = new Expr;

    expr->type = Expr::PREFIX;
    const Token::Type token_type = tokens.front().type;
    tokens.pop_front();

    Expr::Prefix::Operation operation;
    if (token_type == Token::MINUS) operation = Expr::Prefix::NEG;
    else if (token_type == Token::NOT) operation = Expr::Prefix::NOT;

    expr->data = Expr::Prefix{operation, parse_expr(8)}; // 8 is prefix precedence

    return expr;
}

////////////////////////////////////////////////////////////////////////////////

Expr* Parser::parse_expr_group() {
    tokens.pop_front();

    Expr* expr = parse_expr(-1);

    if (tokens.front().type != Token::RPAREN)
        err::fatal("Could not find matching ')'");
    tokens.pop_front();

    return expr;
}

////////////////////////////////////////////////////////////////////////////////

Expr* Parser::parse_expr_if() {
    tokens.pop_front(); // remove if token

    Expr* expr = new Expr;
    expr->type = Expr::IF;

    if (tokens.front().type != Token::LPAREN)
        err::fatal("Missing '(' in if expression");
    tokens.pop_front();

    Expr::If ifval;
    ifval.condition = parse_expr(-1);

    if (tokens.front().type != Token::RPAREN)
        err::fatal("Missing ')' in if expression");
    tokens.pop_front();
}

////////////////////////////////////////////////////////////////////////////////

Expr* Parser::parse_expr_infix(Expr* left) {
    Expr* expr = new Expr;

    expr->type = Expr::INFIX;

    const Token::Type token_type = tokens.front().type;
    tokens.pop_front();

    expr->data = Expr::Infix{
        OPERATIONS.at(token_type), left, parse_expr(PRECEDENCES.at(token_type))
    };

    return expr;
}

////////////////////////////////////////////////////////////////////////////////

int main() {

}