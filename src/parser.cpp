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

Expr* parse_expr(const int precedence) {

}

////////////////////////////////////////////////////////////////////////////////

int main() {

}