#ifndef LEX_H
#define LEX_H

#include <stdint.h>

#define MAX_IDENT_LENGTH 64

typedef enum {
    TOK_ILLEGAL, TOK_EOF,

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
        double nr; char* str;
    } literal;
    TextPos text_pos;
} Tok;

typedef struct {
    const char* in; size_t pos;
    TextPos text_pos;
} Lex;

enum token_type {
    TOKEN_ILLEGAL,
    TOKEN_EOF,
    TOKEN_IDENT,
    TOKEN_INT,
    TOKEN_FUNCTION,
    TOKEN_LET,
    TOKEN_TRUE,
    TOKEN_FALSE, 
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_BANG,
    TOKEN_ASTERISK,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_LT,
    TOKEN_LTE,
    TOKEN_GT,
    TOKEN_GTE,
    TOKEN_EQ,
    TOKEN_NOT_EQ,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_STRING,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    TOKEN_COLON,
};

struct token {
    enum token_type type;
    char literal[MAX_IDENT_LENGTH];
    const char *start;
    const char *end;

    uint32_t line;
    uint32_t pos;
};

struct lexer {
    const char *input;

    /* pos in file */
    uint32_t pos;

    /* line number */
    uint32_t cur_lineno;
    
    /* position in line */
    uint32_t cur_linepos;
};

int gettoken(struct lexer *l, struct token *t);
struct lexer new_lexer(const char *input);
const char *token_type_to_str(enum token_type type);

#endif // LEX_H