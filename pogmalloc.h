#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef POGMALLOC_POGMALLOC_H
#define POGMALLOC_POGMALLOC_H

typedef struct {
    uintptr_t* start;
    uintptr_t* end;
    size_t size;
    int (*expand_function)(size_t words);
} pog_metadata;

static pog_metadata* metadata = NULL;

typedef struct {
    uintptr_t* start;
    size_t size;
} pog_chunk;

static pog_chunk* alloced_chunks = NULL;
static size_t alloced_curr_size = {0};
static size_t alloced_max_size = {0};

static pog_chunk* freed_chunks = NULL;
static size_t freed_curr_size = {0};
static size_t freed_max_size = {0};

pog_chunk* pog_chunk_first_free(size_t size_words, pog_chunk* chunks, size_t chunks_count);
void pog_chunk_insert(pog_chunk to_insert, pog_chunk* chunks, size_t* chunks_count, size_t chunks_max);
void pog_chunk_compress(pog_chunk* chunks, size_t* chunks_count, size_t chunks_max);

void pog_init(uintptr_t* heap_start, size_t heap_size,
              pog_chunk* alloced_chunks_start, size_t alloced_chunks_size,
              pog_chunk* freed_chunks_start, size_t freed_chunks_size,
              int (*expand_function)(size_t words));
void* pog_malloc(size_t size_bytes);
void pog_free(void* ptr);
void* pog_realloc(void* ptr, size_t size_bytes);

#endif //POGMALLOC_POGMALLOC_H
