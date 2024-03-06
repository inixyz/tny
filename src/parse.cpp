#include "parse.hpp"

#include <list>

#include "lex.hpp"

////////////////////////////////////////////////////////////////////////////////

Parser::Parser(const std::list<Tok> toks) {
    this->toks = toks;
}

////////////////////////////////////////////////////////////////////////////////

std::list<Stmt> Parser::get_stmts() {
    std::list<Stmt> stmts;

    while (toks.front().type != Tok::END)
        stmts.push_back(parse_stmt());

    return stmts;
}

////////////////////////////////////////////////////////////////////////////////

Stmt Parser::parse_stmt() {
    switch (toks.front().type) {
    case Tok::LET: return parse_let();
    }
}

////////////////////////////////////////////////////////////////////////////////

Stmt Parser::parse_let() {
    toks.pop_front();

    if (toks.front().type != Tok::IDENT)
        err::fatal("Missing identifier after 'let' keyword");

    const std::string ident = std::get<std::string>(toks.front().literal);
    toks.pop_front();

    if (toks.front().type != Tok::ASSIGN)
        err::fatal("Expected '=' after '" + ident + "'");

    Expr* expr = parse_expr(-1);

    if (toks.front().type != Tok::SEMICOLON)
        err::fatal("Expected ';' after expression");
}

////////////////////////////////////////////////////////////////////////////////

Expr* parse_expr(const int precedence) {

}

////////////////////////////////////////////////////////////////////////////////

int main() {

}