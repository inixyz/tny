#include "lex.h"

#include "err.h"

////////////////////////////////////////////////////////////////////////////////

static inline char next_ch(Lex* const lex) {
    lex->text_pos.column++;
    return lex->in[lex->pos++];
}

////////////////////////////////////////////////////////////////////////////////

static inline void check_line_advance(Lex* const lex, const char ch) {
    if (ch == '\n') {
        lex->text_pos.line++;
        lex->text_pos.column = 1;
    }
}

////////////////////////////////////////////////////////////////////////////////

static inline bool next_if(Lex* const lex, const char ch) {
    return lex->in[lex->pos] == ch ? next_ch(lex) : false;
}

////////////////////////////////////////////////////////////////////////////////

static inline bool is_str(const char ch) {
    return ch != '"';
}

////////////////////////////////////////////////////////////////////////////////

static inline bool is_nr(const char ch) {
    return isxdigit(ch) || strchr("inptyINPTY", ch);
}

////////////////////////////////////////////////////////////////////////////////

static inline bool is_ident(const char ch) {
    return isalpha(ch) || isdigit(ch) || ch == '_';
}

////////////////////////////////////////////////////////////////////////////////

char* next_str_rule(Lex* const lex, bool (*rule)(const char)) {
    char ch = lex->in[lex->pos - 1];
    const size_t start = lex->pos - rule(ch);

    do {
        ch = next_ch(lex);
        check_line_advance(lex, ch);
    } while (rule(ch) && ch != '\0');

    if (ch == '\0') err(lex->text_pos, "Unexpected terminating character");
    else if (!rule(ch)) err(lex->text_pos, "Unexpected '%c' in sequence", ch);

    const size_t size = lex->pos - start;
    char* str = strncpy(malloc(size), lex->in + start, size);
    str[size - 1] = '\0';
    return str;
}

////////////////////////////////////////////////////////////////////////////////

TokType identify_keyword(const char* literal) {
    static const char* KEYWORDS[] = {
        "let", "fn", "return", "if", "else", "while", "for", "break", "continue"
    };
    static const size_t NR_KEYWORDS = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

    for (size_t i = 0; i < NR_KEYWORDS; i++)
        if (!strcmp(literal, KEYWORDS[i])) return TOK_LET + i;

    return TOK_IDENT;
}

////////////////////////////////////////////////////////////////////////////////

Tok next_tok(Lex* const lex) {
    char ch = next_ch(lex);

    // skip whitespace
    while (isspace(ch)) {
        check_line_advance(lex, ch);
        ch = next_ch(lex);
    }

    Tok tok;
    switch (ch) {
    // single
    case ',': tok.type = TOK_COMMA; break;
    case ';': tok.type = TOK_SEMICOLON; break;
    case '(': tok.type = TOK_LPAREN; break;
    case ')': tok.type = TOK_RPAREN; break;
    case '[': tok.type = TOK_LBRACKET; break;
    case ']': tok.type = TOK_RBRACKET; break;
    case '{': tok.type = TOK_LBRACE; break;
    case '}': tok.type = TOK_RBRACE; break;
    case '+': tok.type = TOK_SUM; break;
    case '-': tok.type = TOK_SUB; break;
    case '*': tok.type = TOK_MUL; break;
    case '/': tok.type = TOK_DIV; break;
    case '%': tok.type = TOK_MOD; break;

    // double
    case '=': tok.type = next_if(lex, '=') ? TOK_EQ : TOK_ASSIGN; break;
    case '!': tok.type = next_if(lex, '=') ? TOK_NEQ : TOK_NOT; break;
    case '>': tok.type = next_if(lex, '=') ? TOK_GTEQ : TOK_GT; break;
    case '<': tok.type = next_if(lex, '=') ? TOK_LTEQ : TOK_LT; break;
    case '&':
        if (next_if(lex, '&')) tok.type = TOK_AND;
        else err(lex->text_pos, "Invalid token '&%c'", ch);
        break;

    case '|':
        if (next_if(lex, '|')) tok.type = TOK_OR;
        else err(lex->text_pos, "Invalid token '|%c'", ch);
        break;

    case '"':
        tok.type = TOK_STR;
        tok.literal.str = next_str_rule(lex, is_str);
        break;

    case '0' ... '9':
        tok.type = TOK_NR;
        tok.literal.nr = atof(next_str_rule(lex, is_nr));
        break;

    case 'a' ... 'z': case 'A' ... 'Z': case '_':
        tok.literal.str = next_str_rule(lex, is_ident);
        tok.type = identify_keyword(tok.literal.str);
        break;

    case '\0': tok.type = TOK_EOF; break;
    default:
        err(lex->text_pos, "Could not match any token for character '%c'", ch);
    }

    tok.text_pos = lex->text_pos;
    return tok;
}

////////////////////////////////////////////////////////////////////////////////

Lex lex_new(const char* const in) {
    return (Lex){in, 0, (TextPos){1, 1}};
}

////////////////////////////////////////////////////////////////////////////////