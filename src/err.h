#ifndef ERR_H
#define ERR_H

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

typedef struct {
    size_t line, column;
} TextLocation;

__attribute__((noreturn))
static inline void err(const TextLocation location, const char* const format,
    ...){

    fprintf(stderr, "%lu:%lu: error: ", location.line, location.column);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

#endif // ERR_H