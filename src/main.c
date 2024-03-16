#include <stdio.h>

typedef struct Cons{
    enum {ATOM_NUM, ATOM_SYMBOL} type;
    union {
        Num num; char* symbol; struct {struct Cons *car, *cdr;};
    };
} Cons;

int main() {
    Cons item;
    Cons boss;
    boss.car = &item;

    printf("%lu\n", sizeof(item));
}
