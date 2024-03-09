#include "lexer.hpp"

// C++ headers
#include <string>
#include <list>
#include <iostream>
#include <regex>

// internal headers
#include "err.hpp"

////////////////////////////////////////////////////////////////////////////////

Lexer::Lexer(const std::string in) {
    this->in = in;
}

////////////////////////////////////////////////////////////////////////////////

std::list<Token> Lexer::get_tokens() {
    std::list<Token> tokens;
    do {
        tokens.push_back(next_token());
    } while (tokens.back().type != Token::END);
    return tokens;
}

////////////////////////////////////////////////////////////////////////////////

char Lexer::next_ch() {
    const char ch = in.front();
    in.erase(in.begin());
    return ch;
}

////////////////////////////////////////////////////////////////////////////////

bool Lexer::next_if(const char ch) {
    if (in.front() == ch) {
        in.erase(in.begin());
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

std::string Lexer::next_match(const std::regex regex) {
    std::string match;
    while (std::regex_match(in.substr(0, 1), regex)) match += next_ch();
    return match;
}

////////////////////////////////////////////////////////////////////////////////

Token Lexer::next_token() {
    if (in.empty()) return {Token::END};

    char ch = next_ch();

    // skip whitespace
    while (ch == ' ' || ch == '\t' || ch == '\n') ch = next_ch();

    switch(ch) {
    //single
    case ',': return {Token::COMMA};
    case ';': return {Token::SEMICOLON};
    case '(': return {Token::LPAREN};
    case ')': return {Token::RPAREN};
    case '[': return {Token::LBRACKET};
    case ']': return {Token::RBRACKET};
    case '{': return {Token::LBRACE};
    case '}': return {Token::RBRACE};
    case '+': return {Token::PLUS};
    case '-': return {Token::MINUS};
    case '*': return {Token::ASTERISK};
    case '/': return {Token::SLASH};
    case '%': return {Token::PERCENT};

    // double
    case '=': return {next_if('=') ? Token::EQ  : Token::ASSIGN};
    case '!': return {next_if('=') ? Token::NEQ : Token::NOT};
    case '>': return {next_if('=') ? Token::GEQ : Token::GT};
    case '<': return {next_if('=') ? Token::LEQ : Token::LT};
    case '&': {
        if ((ch = next_ch()) == '&') return {Token::AND};
        err::fatal("Invalid token &" + ch);
    }
    case '|': {
        if ((ch = next_ch()) == '|') return {Token::AND};
        err::fatal("Invalid token |" + ch);
    }

    case '"': {
        if (in.find('"') == std::string::npos)
            err::fatal("Could not find matching \"");

        const std::string literal = in.substr(0, in.find('"'));
        in.erase(0, literal.size() + 1);
        return {Token::STR, literal};
    }

    case 'a' ... 'z': case 'A' ... 'Z': case '_': {
        const std::string literal = ch + next_match(std::regex("[_a-zA-Z0-9]"));

        if (KEYWORDS.find(literal) != KEYWORDS.end())
            return {KEYWORDS.at(literal)};
        return {Token::IDENT, literal};
    }

    case '0' ... '9': {
        std::string literal = ch + next_match(std::regex("[_.0-9a-hox]"));
        std::erase(literal, '_');

        size_t err_pos = 0;
        const float val = std::stof(literal, &err_pos);
        if (err_pos == literal.size()) return {Token::FLOAT, val};

        err::fatal("Invalid number " + literal);
    }

    default: err::fatal("Could not match any token for character " + ch);
    }

    return {Token::END}; // unreachable
}

////////////////////////////////////////////////////////////////////////////////