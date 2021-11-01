#include <stdio.h>
#include <string.h>
#include "pog_static_heap.h"
#include "pog_real_heap.h"

char* x;

#define STATIC_MEM_CAP 2048
uintptr_t static_mem[STATIC_MEM_CAP];

int main() {
    pog_gc_init(&static_mem_ptrs, sizeof(static_mem));

    pog_gc_mark_static(&x);

    //pog_real_heap_init();

    pog_static_heap_init();

    x = pog_malloc(1);

    pog_malloc(80);
    pog_malloc(80);
    pog_malloc(80);

    char* a = pog_malloc(sizeof(char) * 5);
    strcpy(a, "Hello");
    printf("%s\n", a);

    char* b = pog_malloc(sizeof(char) * 10);
    strcpy(b, " world foo");
    printf("%s\n", b);

    char* c = pog_malloc(sizeof(char) * 2);
    strcpy(c, "!\n");
    printf("%s\n", c);

    pog_debug();

    pog_gc_collect();

    pog_debug();

    x = NULL;
    pog_gc_collect();
    pog_debug();
    pog_squash();
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
