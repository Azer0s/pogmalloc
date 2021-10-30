#include <stdio.h>
#include <string.h>
#include "pog_static_heap.h"

int main() {
    pog_static_heap_init();

    char* a = pog_malloc(sizeof(char) * 5);
    strcpy(a, "Hello");

    printf("%s", a);

    //pog_free(a);

    return 0;
}
