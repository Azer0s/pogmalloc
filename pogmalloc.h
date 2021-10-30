#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef POGMALLOC_POGMALLOC_H
#define POGMALLOC_POGMALLOC_H

typedef struct {
    uintptr_t* start;
    uintptr_t* end;
    size_t size;
    int (*expand_function)(size_t words, size_t* alloced_chunks_size, size_t* freed_chunks_size);
} pog_metadata;

static pog_metadata* metadata = NULL;

typedef struct {
    uintptr_t* start;
    size_t size;
} pog_chunk;

typedef struct {
    pog_chunk* chunks;
    size_t curr_size;
    size_t max_size;
} pog_chunk_list;

static pog_chunk_list alloced_chunks_list = {0};
static pog_chunk_list freed_chunks_list = {0};

size_t pog_chunk_first_free(pog_chunk_list* list, size_t size_words);
void pog_chunk_insert(pog_chunk_list* list, pog_chunk to_insert);
void pog_chunk_remove(pog_chunk_list* list, size_t index);
void pog_chunk_compress(pog_chunk_list* chunks);

void pog_init(uintptr_t* heap_start, size_t heap_size,
              pog_chunk* alloced_chunks_start, size_t alloced_chunks_size,
              pog_chunk* freed_chunks_start, size_t freed_chunks_size,
              int (*expand_function)(size_t words, size_t* alloced_chunks_size, size_t* freed_chunks_size));
void* pog_malloc(size_t size_bytes);
void pog_free(void* ptr);
void* pog_realloc(void* ptr, size_t size_bytes);

#endif //POGMALLOC_POGMALLOC_H
