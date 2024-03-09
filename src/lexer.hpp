#pragma once

// C++ headers
#include <variant>
#include <string>
#include <list>
#include <regex>

struct Token {
    enum Type {
        END,

        // delimiters
        COMMA, SEMICOLON, LPAREN, RPAREN, LBRACKET, RBRACKET, LBRACE, RBRACE,

        // types
        IDENT, FLOAT, STR,

        // keywords
        LET, IF, ELSE, WHILE, FUNC, RETURN,

        // arithmetic operators
        ASSIGN, PLUS, MINUS, ASTERISK, SLASH, PERCENT,

        // relational operators
        EQ, NEQ, GT, LT, GEQ, LEQ,

        // logical operators
        AND, OR, NOT
    } type;

    std::variant<float, std::string> literal;
};

class Lexer {
public:
    Lexer(const std::string in);
    std::list<Token> get_tokens();

private:
    std::string in;

    char next_ch();
    bool next_if(const char ch);
    std::string next_match(const std::regex regex);
    Token next_token();

    const std::unordered_map<std::string, Token::Type> KEYWORDS = {
        {"let", Token::LET}, {"if", Token::IF}, {"else", Token::ELSE},
        {"while", Token::WHILE}, {"func", Token::FUNC}, {"return", Token::RETURN}
    };
};