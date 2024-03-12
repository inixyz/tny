#pragma once 

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "lex.h"

typedef enum {
    EXPR_IDENT, EXPR_NR, EXPR_STR, EXPR_LIST, EXPR_INDEX, EXPR_ASSIGN,
    EXPR_PREFIX, EXPR_INFIX, EXPR_FN, EXPR_CALL, EXPR_IF, EXPR_WHILE, EXPR_FOR,
} ExprType;

typedef struct {
    size_t size; Expr* items[];
} ExprList;

typedef struct {
    Expr *left, *index;
} ExprIndex;

// TODO pair up structs with the same fields

typedef struct {
    Expr *left, *right;
} ExprAssign;

typedef struct {
    Expr* right;
} ExprPrefix;

typedef enum {
    STMT_LET, STMT_RETURN, STMT_BREAK, STMT_CONTINUE, STMT_EXPR
} StmtType;

enum operator {
    OP_UNKNOWN,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_GT,
    OP_GTE,
    OP_LT,
    OP_LTE,
    OP_EQ,
    OP_NOT_EQ,
    OP_NEGATE,
    OP_AND,
    OP_OR,
    OP_MODULO,
};

struct postfix_expression {
    enum operator operator;
    struct expression *left;
};

struct infix_expression {
    enum operator operator;
    struct expression *left;
    struct expression *right;
};

struct identifier {
    char value[MAX_IDENT_LENGTH];
    struct token token;
};

struct identifier_list {
    struct identifier *values;
    uint32_t size;
    uint32_t cap;
};

struct statement {
    enum statement_type type;
    struct token token;
    struct identifier name;
    struct expression *value;
};

struct block_statement {
    struct token token;
    struct statement *statements;
    uint32_t size;
    uint32_t cap;
};

struct if_expression {
    struct expression *condition;
    struct block_statement *consequence;
    struct block_statement *alternative;
};

struct function_literal {
    char name[MAX_IDENT_LENGTH];
    struct identifier_list parameters;
    struct block_statement *body;
};



struct call_expression {
    struct expression *function;
    struct expression_list arguments;
};



struct index_expression {
    struct expression *left;
    struct expression *index;
};

struct slice_expression {
    struct expression *left;
    struct expression *start;
    struct expression *end;
};

struct while_expression {
    struct expression *condition;
    struct block_statement *body;
};

struct for_expression {
    struct statement init;
    struct expression* condition;
    struct statement inc;
    struct block_statement *body;
};

typedef struct {
    ExprType type;
} Expr;

struct expression {
    enum expression_type type;
    struct token token;
    union {
        int64_t integer;
        bool boolean;
        char *string;
        struct identifier ident;
        struct prefix_expression prefix;
        struct postfix_expression postfix;
        struct infix_expression infix;
        struct if_expression ifelse;
        struct function_literal function;
        struct call_expression call;
        struct expression_list array;
        struct index_expression index;
        struct slice_expression slice;
        struct while_expression while_loop;
        struct assignment_expression assign;
        struct for_expression for_loop;
    };
};

struct program {
    struct statement *statements;
    uint32_t cap;
    uint32_t size;
};

struct parser {
    struct lexer *lexer;
    struct token current_token;
    struct token next_token;

    uint32_t errors;
    char** error_messages;
};

struct parser new_parser(struct lexer *l);
struct program *parse_program(struct parser *parser);
struct program *parse_program_str(const char *str);
void block_statement_to_str(char *str, const struct block_statement *b);
void identifier_list_to_str(char *str, const struct identifier_list *identifiers);
char *program_to_str(const struct program *p);
void free_program(struct program *p);
char *operator_to_str(const enum operator operator);
const char* expression_type_to_str(enum expression_type);
