#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#if FEATURE_DEBUG
#include <printf.h>

#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define DEBUG(format, ...) \
printf(ANSI_COLOR_YELLOW "[DEBUG] %s:%d@%s ",  __func__, __LINE__, __FILE__); \
printf(format ANSI_COLOR_RESET, __VA_ARGS__)

#define TRACE(format, ...) \
printf(ANSI_COLOR_CYAN "[TRACE] %s:%d@%s ",  __func__, __LINE__, __FILE__); \
printf(format ANSI_COLOR_RESET, __VA_ARGS__)
#else
#define DEBUG(val, ...)
#define TRACE(val, ...)
#endif

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
static pog_chunk_list tmp_chunks_list = {0};
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
 * @param tmp_chunks_start the start pointer of the temporary chunks array (used for squashing, gc, etc.)
 * @param tmp_chunks_size the size of the temporary chunks array
 * @param expand_function the function to expand the heap (also expects the freed chunks array to increase)
 */
void pog_init(uintptr_t* heap_start, size_t heap_size,
              pog_chunk* alloced_chunks_start, size_t alloced_chunks_size,
              pog_chunk* freed_chunks_start, size_t freed_chunks_size,
              pog_chunk* tmp_chunks_start, size_t tmp_chunks_size,
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

/**
 * Prints the alloced and freed memory chunks
 */
void pog_debug();

#ifdef FEATURE_GC
const uintptr_t* stack_base;

uintptr_t** static_mem_ptrs;
size_t static_mem_curr_size;
size_t static_mem_max_size;

void pog_gc_static_init(uintptr_t** static_mem_start);

#define pog_gc_init(mem_start, mem_cap) \
stack_base = (uintptr_t*) __builtin_frame_address(0); \
static_mem_max_size = mem_cap;          \
pog_gc_static_init((uintptr_t **) (mem_start))
#else
#define pog_gc_init(static_memory_start, static_memory_size) assert(0 && "FEATURE_GC is not enabled")
#endif

void pog_gc_mark_static(void* mem);
void pog_gc_collect();

#pragma endregion pogmalloc

#endif //POGMALLOC_POGMALLOC_H
