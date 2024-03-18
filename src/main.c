#include <stdio.h>
#include <stdlib.h>

typedef struct Cons{
    enum {ATOM_NUM, ATOM_SYMBOL} type;
    union {
        Num num; char* symbol; struct {struct Cons *car, *cdr;};
    };
} Cons;

void print_cons(const Cons* const cons) {
    if (cons->type == ATOM_NUM) printf("%g", cons->num);
    else if (cons->type == ATOM_SYMBOL) printf("%s", cons->symbol);
    else {
        if (cons->car->type != ATOM_NUM && cons->car->type != ATOM_SYMBOL)
            printf("(");
        print_cons(cons->car);
        if (cons->car->type != ATOM_NUM && cons->car->type != ATOM_SYMBOL)
            printf(")");

        if (cons->cdr->type == ATOM_NUM || cons->cdr->type == ATOM_SYMBOL)
            printf(" . ");
        else printf(" ");

        if (cons->cdr->type != ATOM_NUM && cons->cdr->type != ATOM_SYMBOL) {
            if (cons->cdr->car == NULL && cons->cdr->cdr == NULL) {
                return;
            } else print_cons(cons->cdr);
        }
        else print_cons(cons->cdr);
    }
}

void print_list(const Cons* const cons) {
    printf("(");
    print_cons(cons);
    printf(")\n");
}

Cons* make_atom_num(const Num num) {
    Cons* cons = malloc(sizeof(Cons));
    cons->type = ATOM_NUM;
    cons->num = num;
    return cons;
}

Cons* make_cons(Cons* const car, Cons* const cdr) {
    Cons* cons = malloc(sizeof(Cons));
    cons->type = 100;
    cons->car = car;
    cons->cdr = cdr;
    return cons;
}



int main() {
    Cons* test = make_cons(make_cons(make_atom_num(1), make_atom_num(2)), make_cons(make_atom_num(3), make_atom_num(4)));
    print_list(test);

    Cons* test2 = make_cons(make_atom_num(1), make_cons(make_atom_num(2),  make_cons(NULL, NULL)));
    print_list(test2);
}
