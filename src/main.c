#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    enum {TYPE_LIST, TYPE_IDENT, TYPE_NUM, TYPE_STR} type;
    union {
        struct List* list; char* ident; Num num; char* str;
    };
} Data;

typedef struct List {
    struct List* next; Data data;
} List;

List* list_push_front(List* const list, const Data data) {
    List* head = calloc(1, sizeof(*head));
    *head = (List){list, data};
    return head;
}

void list_print(const List* const list) {
    if (list->data.type == TYPE_IDENT) printf("%s", list->data.ident);
    else if (list->data.type == TYPE_NUM) printf("%g", list->data.num);
    else if (list->data.type == TYPE_STR) printf("\"%s\"", list->data.str);
    else {
        printf("["); list_print(list->data.list); printf("]");
    }

    if (list->next) {
        printf(", "); list_print(list->next);
    }
}

int main() {
    Data data = {TYPE_NUM, .num = 1};
    List* head = list_push_front(NULL, data);

    data = (Data){TYPE_IDENT, .ident = "boss"};
    head = list_push_front(head, data);

    data = (Data){TYPE_LIST, .list = list_push_front(NULL, (Data){TYPE_NUM, .num = 3})};
    head = list_push_front(head, data);
    head->data.list = list_push_front(head->data.list, (Data){TYPE_STR, .str = "salut"});

    printf("["); list_print(head); printf("]\n");

    // char str[] ="(eval    (+ 1 2))";
    // //char test[1000];
    // strcpy(test, str);

    // // { + 1 { + 2 3 } }

    // // (+ 1 (+ 2 3))





    // char * pch;
    // printf ("Splitting string \"%s\" into tokens:\n",str);
    // char* curr;
    // pch = strtok_r(str, " ()", &curr);
    // while (pch != NULL)
    // {
    //     printf ("%s | '%c'\n",pch, test[curr - str - 1]);
    //     pch = strtok_r(NULL, " ()", &curr);
    // }
}