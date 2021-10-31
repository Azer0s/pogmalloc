#include <stdio.h>
#include <string.h>
#include "pog_static_heap.h"
#include "pog_real_heap.h"

int main() {
    //pog_real_heap_init();

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
    pog_free(b);
    pog_free(c);

    pog_debug();

    pog_squash();

    pog_debug();

    return 0;
}
