#pragma once

#include <variant>
#include <string>
#include <list>
#include <regex>

struct Tok {
    enum Type {
        END,

        // symbols
        COMMA, SEMICOLON, LPAREN, RPAREN, LBRACKET, RBRACKET, LBRACE, RBRACE,

        // types
        IDENT, INT, FLOAT, STR,

        // keywords
        LET, TRUE, FALSE, IF, ELSE, WHILE, FUNC, RET,

        // arithmetic operators
        ASSIGN, ADD, SUB, MUL, DIV, MOD,

        // relational operators
        EQ, NEQ, GT, LT, GEQ, LEQ,

        // logical operators
        AND, OR, NOT
    } type;

    std::variant<int, float, std::string> literal;
};

class Lex {
public:
    Lex(const std::string in);
    std::list<Tok> get_toks();

private:
    std::string in;

    char next_ch();
    bool next_if(const char ch);
    std::string next_match(const std::regex regex);
    Tok next_tok();

    const std::unordered_map<std::string, Tok::Type> KEYWORDS = {
        {"let", Tok::LET}, {"true", Tok::TRUE}, {"false", Tok::FALSE},
        {"if", Tok::IF}, {"else", Tok::ELSE}, {"while", Tok::WHILE},
        {"func", Tok::FUNC}, {"return", Tok::RET}
    };
};