#ifndef LEX_H
#define LEX_H

#include <stddef.h>

#include "err.h"

typedef enum {
    TOK_EOF,

    // delimiters
    TOK_COMMA, TOK_SEMICOLON, TOK_LPAREN, TOK_RPAREN, TOK_LBRACKET,
    TOK_RBRACKET, TOK_LBRACE, TOK_RBRACE,

    // types
    TOK_IDENT, TOK_NR, TOK_STR,

    // keywords
    TOK_LET, TOK_FN, TOK_RETURN, TOK_IF, TOK_ELSE, TOK_WHILE, TOK_FOR,
    TOK_BREAK, TOK_CONTINUE,

    // arithmetic operators
    TOK_ASSIGN, TOK_SUM, TOK_SUB, TOK_MUL, TOK_DIV, TOK_MOD,

    // relational operators
    TOK_EQ, TOK_NEQ, TOK_GT, TOK_LT, TOK_GTEQ, TOK_LTEQ,

    // logical operators
    TOK_AND, TOK_OR, TOK_NOT
} TokType;

typedef struct {
    TokType type;
    union {
        NrType nr; char* str;
    };
    TextLocation location;
} Tok;

typedef struct {
    const char* in; size_t pos;
    TextLocation location;
} Lex;

Lex lex_new(const char* const in);
Tok lex_next_tok(Lex* const lex);

#endif // LEX_H