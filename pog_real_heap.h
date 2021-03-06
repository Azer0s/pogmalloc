#include <unistd.h>
#include "pogmalloc.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#ifndef POGMALLOC_POG_REAL_HEAP_H
#define POGMALLOC_POG_REAL_HEAP_H

#define HEAP_CAP_REAL_CHUNKS 8192
static pog_chunk real_alloced[HEAP_CAP_REAL_CHUNKS] = {0};
static pog_chunk real_freed[HEAP_CAP_REAL_CHUNKS] = {0};
static pog_chunk real_tmp[HEAP_CAP_REAL_CHUNKS] = {0};
static size_t real_heap_size_words = 64;

int real_heap_expand(size_t words, size_t* alloced_chunks_size, size_t* freed_chunks_size) {
    *alloced_chunks_size += 1;
    *freed_chunks_size += 1;

    assert(*alloced_chunks_size < HEAP_CAP_REAL_CHUNKS);
    assert(*freed_chunks_size < HEAP_CAP_REAL_CHUNKS);

    void* old_sbrk = sbrk(0);
    sbrk((int)words * (int)sizeof(uintptr_t));
    void* new_sbrk = sbrk(0);

    if (old_sbrk < new_sbrk) {
        return 0;
    }

    return 1;
}

void pog_real_heap_init() {
    void* heap_start = sbrk(0);
    sbrk((int)real_heap_size_words * (int)sizeof(uintptr_t));

    void* new_sbrk = sbrk(0);
    assert(new_sbrk > heap_start);

    pog_init(heap_start, real_heap_size_words,
             &real_alloced[0], real_heap_size_words * 8,
             &real_freed[0], real_heap_size_words * 8,
             &real_tmp[0], real_heap_size_words * 8,
             &real_heap_expand);

    //HACK: sometimes, sbrk(0) gives back a weird address space; leak two words to properly align brk
    //idk why this works and at this point I'm too afraid to ask
    pog_malloc(16);
}


#endif //POGMALLOC_POG_REAL_HEAP_H

#pragma clang diagnostic pop