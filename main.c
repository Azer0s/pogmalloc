#include <stdio.h>
#include <string.h>
#include "pog_static_heap.h"

int main() {
    pog_static_heap_init();

    char* a = pog_malloc(sizeof(char) * 5);
    strcpy(a, "Hello");
    printf("%s", a);

    char* b = pog_malloc(sizeof(char) * 6);
    strcpy(b, " world");
    printf("%s", b);

    char* c = pog_malloc(sizeof(char) * 2);
    strcpy(c, "!\n");
    printf("%s", c);

    pog_debug();

    pog_free(a);

    pog_debug();

    return 0;
}
