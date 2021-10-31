#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <printf.h>

#ifndef POGMALLOC_POGMALLOC_H
#define POGMALLOC_POGMALLOC_H

#pragma region data
typedef int (*expand_function_t)(size_t words, size_t* alloced_chunks_size, size_t* freed_chunks_size);

typedef struct {
    uintptr_t* start;
    uintptr_t* end;
    size_t size;
    void* expand_function;
} pog_metadata;

static pog_metadata metadata = {0};

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
static pog_chunk_list freed_tmp_chunks_list = {0};
#pragma endregion data

#pragma region pog_chunk
size_t pog_chunk_first_free(pog_chunk_list* list, size_t size_words);
size_t pog_chunk_by_ptr(pog_chunk_list* list, void* ptr);
void pog_chunk_insert(pog_chunk_list* list, pog_chunk to_insert);
void pog_chunk_remove(pog_chunk_list* list, size_t index);
void pog_chunk_squash(pog_chunk_list* dst, pog_chunk_list* src);
void pog_chunk_debug(pog_chunk_list list, const char *name);
#pragma endregion pog_chunk

#pragma region pogmalloc
/**
 * Initializes the pogmalloc allocator
 * @param heap_start the start pointer of the heap
 * @param heap_size the heap size (the end pointer will be heap_start + heap_size)
 * @param alloced_chunks_start the start pointer of the allocated chunks array
 * @param alloced_chunks_size the size of the allocated chunks array
 * @param freed_chunks_start the start pointer of the freed chunks array
 * @param freed_chunks_size the size of the freed chunks array
 * @param freed_tmp_chunks_start the start pointer of the temporary freed chunks array
 * @param freed_tmp_chunks_size the size of the temporary freed chunks array
 * @param expand_function the function to expand the heap (also expects the freed chunks array to increase)
 */
void pog_init(uintptr_t* heap_start, size_t heap_size,
              pog_chunk* alloced_chunks_start, size_t alloced_chunks_size,
              pog_chunk* freed_chunks_start, size_t freed_chunks_size,
              pog_chunk* freed_tmp_chunks_start, size_t freed_tmp_chunks_size,
              expand_function_t expand_function);

/**
 * Allocates a chunk of memory on the heap
 * @param size_bytes the size to allocate in bytes (will be padded to next highest WORD)
 * @return the pointer to the start of the allocated memory
 */
void* pog_malloc(size_t size_bytes);

/**
 * Frees the memory held by a pointer
 * @param ptr the pointer to free memory of
 */
void pog_free(void* ptr);

/**
 * Squashes the array of freed chunks to make later allocations faster
 */
void pog_squash();

/**
 * Reallocates (resizes) the memory held by a pointer to a new size.
 * If the new size is smaller than the current size, the end of the memory chunk will be cut and freed.
 * If the new size is larger than the current size, a new chunk of memory will be allocated, the memory will be
 * copied via memcpy and the old chunk will be freed.
 * @param ptr the pointer to the memory chunk to resize
 * @param size_bytes the new size in bytes
 * @return
 */
void* pog_realloc(void* ptr, size_t size_bytes);

void pog_debug();
#pragma endregion pogmalloc

#endif //POGMALLOC_POGMALLOC_H
