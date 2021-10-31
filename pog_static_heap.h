#include <assert.h>
#include "pogmalloc.h"

#ifndef POGMALLOC_POG_STATIC_HEAP_H
#define POGMALLOC_POG_STATIC_HEAP_H

#ifndef HEAP_CAP_BYTES
    #define HEAP_CAP_BYTES 64000
#endif

static_assert(HEAP_CAP_BYTES % sizeof(uintptr_t) == 0, "Heap capacity must be divisible by the platforms pointer size!");
#define HEAP_CAP_WORDS (HEAP_CAP_BYTES / sizeof(uintptr_t))

uintptr_t heap[HEAP_CAP_WORDS] = {0};
pog_chunk alloced[HEAP_CAP_WORDS] = {0};
pog_chunk freed[HEAP_CAP_WORDS] = {0};
pog_chunk freed_tmp[HEAP_CAP_WORDS] = {0};

int fixed_heap_expand(size_t words, size_t* alloced_chunks_size, size_t* freed_chunks_size) {
    (void) words;
    (void) alloced_chunks_size;
    (void) freed_chunks_size;

    assert(0 && "Static heap is not expandable");
    return 1;
}

void pog_static_heap_init() {
    pog_init(&heap[0], HEAP_CAP_WORDS,
             &alloced[0], HEAP_CAP_WORDS,
             &freed[0], HEAP_CAP_WORDS,
             &freed_tmp[0], HEAP_CAP_WORDS,
             &fixed_heap_expand);
}

#endif //POGMALLOC_POG_STATIC_HEAP_H
