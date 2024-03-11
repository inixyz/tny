#ifndef ERR_H
#define ERR_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "lex.h"

__attribute__((noreturn))
static inline void err(const TextPos text_pos, const char* const format, ...){
    fprintf(stderr, "%lu:%lu: error: ", text_pos.line, text_pos.column);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

#endif // ERR_H