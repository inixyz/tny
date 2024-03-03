#include "lex.hpp"

#include <string>
#include <list>
#include <iostream>
#include <regex>

#include "err.hpp"

////////////////////////////////////////////////////////////////////////////////

Lex::Lex(const std::string in) {this->in = in;}

////////////////////////////////////////////////////////////////////////////////

std::list<Tok> Lex::get_toks() {
    std::list<Tok> toks;
    do {
        toks.push_back(next_tok());
    } while (toks.back().type != Tok::END);
    return toks;
}

////////////////////////////////////////////////////////////////////////////////

char Lex::next_ch() {
    const char ch = in.front();
    in.erase(in.begin());
    return ch;
}

////////////////////////////////////////////////////////////////////////////////

bool Lex::next_if(const char ch) {
    if (in.front() == ch) {
        in.erase(in.begin());
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

std::string Lex::next_match(const std::regex regex) {
    std::string match;
    while (std::regex_match(in.substr(0, 1), regex)) match += next_ch();
    return match;
}

////////////////////////////////////////////////////////////////////////////////

Tok Lex::next_tok() {
    if (in.empty()) return {Tok::END};

    char ch = next_ch();

    // skip whitespace
    while (ch == ' ' || ch == '\t' || ch == '\n') ch = next_ch();

    switch(ch) {
    //single
    case ',': return {Tok::COMMA};
    case ';': return {Tok::SEMICOLON};
    case '(': return {Tok::LPAREN};
    case ')': return {Tok::RPAREN};
    case '[': return {Tok::LBRACKET};
    case ']': return {Tok::RBRACKET};
    case '{': return {Tok::LBRACE};
    case '}': return {Tok::RBRACE};
    case '+': return {Tok::ADD};
    case '-': return {Tok::SUB};
    case '*': return {Tok::MUL};
    case '/': return {Tok::DIV};
    case '%': return {Tok::MOD};

    // double
    case '=': return {next_if('=') ? Tok::EQ  : Tok::ASSIGN};
    case '!': return {next_if('=') ? Tok::NEQ : Tok::NOT};
    case '>': return {next_if('=') ? Tok::GEQ : Tok::GT};
    case '<': return {next_if('=') ? Tok::LEQ : Tok::LT};
    case '&': {
        if ((ch = next_ch()) == '&') return {Tok::AND};
        err::fatal("Invalid token &" + ch);
    }
    case '|': {
        if ((ch = next_ch()) == '|') return {Tok::AND};
        err::fatal("Invalid token |" + ch);
    }

    case '"': {
        if (in.find('"') == std::string::npos)
            err::fatal("Could not find matching \"");

        const std::string literal = in.substr(0, in.find('"'));
        in.erase(0, literal.size() + 1);
        return {Tok::STR, literal};
    }

    case 'a' ... 'z': case 'A' ... 'Z': case '_': {
        const std::string literal = ch + next_match(std::regex("[_a-zA-Z0-9]"));

        if (KEYWORDS.find(literal) != KEYWORDS.end())
            return {KEYWORDS.at(literal)};
        return {Tok::IDENT, literal};
    }

    case '0' ... '9': {
        std::string literal = ch + next_match(std::regex("[_.0-9a-hox]"));
        std::erase(literal, '_');

        size_t err_pos = 0;
        const int ival = std::stoi(literal, &err_pos, 0);
        if (err_pos == literal.size()) return {Tok::INT, ival};

        const float fval = std::stof(literal, &err_pos);
        if (err_pos == literal.size()) return {Tok::FLOAT, fval};

        err::fatal("Invalid number " + literal);
    }

    default: err::fatal("Could not match any token for character " + ch);
    }

    return {Tok::END}; // unreachable
}

////////////////////////////////////////////////////////////////////////////////