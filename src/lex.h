#ifndef LEX_H
#define LEX_H

#include <stddef.h>

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
    size_t line, column;
} TextPos;

typedef struct {
    TokType type;
    union {
        NrType nr; char* str;
    } literal;
    TextPos text_pos;
} Tok;

typedef struct {
    const char* in; size_t pos;
    TextPos text_pos;
} Lex;

Tok next_tok(Lex* const lex);
Lex lex_new(const char* const in);

#endif // LEX_H