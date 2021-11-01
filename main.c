#include <stdio.h>
#include <string.h>
#include "pog_static_heap.h"
#include "pog_real_heap.h"

int main() {
    pog_gc_init();

    //pog_real_heap_init();

    pog_static_heap_init();

    pog_malloc(80);
    pog_malloc(80);
    pog_malloc(80);

    char* a = pog_malloc(sizeof(char) * 5);
    strcpy(a, "Hello");
    printf("%s", a);

    char* b = pog_malloc(sizeof(char) * 10);
    strcpy(b, " world foo");
    printf("%s", b);

    char* c = pog_malloc(sizeof(char) * 2);
    strcpy(c, "!\n");
    printf("%s", c);

    pog_debug();

    pog_gc_collect();

    pog_debug();

    b = pog_realloc(b, sizeof(char) * 18);
    strcpy(b + 8, "oo bar");
    printf("%s\n", b);

    pog_debug();

    b = pog_realloc(b, sizeof(char) * 10);
    strcpy(b, " world foo");
    printf("%s\n", b);

    pog_debug();

    pog_free(a);
    pog_free(b);
    pog_free(c);

    pog_debug();

    pog_squash();

    pog_debug();

    return 0;
}
