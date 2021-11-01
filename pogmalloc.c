#include <memory.h>
#include "pogmalloc.h"

void pog_init(uintptr_t* heap_start, size_t heap_size,
              pog_chunk* alloced_chunks_start, size_t alloced_chunks_size,
              pog_chunk* freed_chunks_start, size_t freed_chunks_size,
              pog_chunk* tmp_chunks_start, size_t tmp_chunks_size,
              expand_function_t expand_function) {
    metadata = (pog_metadata) {
        .start = heap_start,
        .end = (uintptr_t *) heap_start[heap_size - 1],
        .expand_function = (void*)expand_function
    };

    alloced_chunks_list = (pog_chunk_list) {
        .chunks = alloced_chunks_start,
        .curr_size = 0,
        .max_size = alloced_chunks_size
    };

    freed_chunks_list = (pog_chunk_list) {
        .chunks = freed_chunks_start,
        .curr_size = 0,
        .max_size = freed_chunks_size
    };

    tmp_chunks_list = (pog_chunk_list) {
        .chunks = tmp_chunks_start,
        .curr_size = 0,
        .max_size = tmp_chunks_size
    };

    pog_chunk_insert(&freed_chunks_list, (pog_chunk) {
            .start = heap_start,
            .size = heap_size
    });
}

void *pog_malloc(size_t size_bytes) {
    if (size_bytes == 0) {
        return NULL;
    }

    const size_t size_words = (size_bytes + (sizeof(uintptr_t) - 1)) / sizeof(uintptr_t);

    size_t first_free_chunk_idx = pog_chunk_first_free(&freed_chunks_list, size_words);

    if (first_free_chunk_idx == (size_t) -1) {
        //If no best fit was found, try to compress freed chunks and try again
        pog_squash();
        first_free_chunk_idx = pog_chunk_first_free(&freed_chunks_list, size_words);
    }

    if (first_free_chunk_idx == (size_t) -1) {
        size_t alloced_max_size_before = alloced_chunks_list.max_size;
        size_t freed_max_size_before = freed_chunks_list.max_size;

        //If still no chunks was found, expand the heap space
        expand_function_t fn = (expand_function_t) metadata.expand_function;
        if(fn(size_words, &alloced_chunks_list.max_size, &freed_chunks_list.max_size) != 0) {
            //Something went wrong while expanding the heap space :(
            exit(1);
        }

        //If the expansion was successful, we also expect the freed size to have increased
        //(increasing the size of alloced chunks is optional

        assert(alloced_chunks_list.max_size >= alloced_max_size_before);
        assert(freed_chunks_list.max_size > freed_max_size_before);

        pog_chunk_insert(&freed_chunks_list, (pog_chunk) {
            .start = (uintptr_t *) (freed_chunks_list.chunks + freed_chunks_list.max_size),
            .size = size_words
        });

        first_free_chunk_idx = pog_chunk_first_free(&freed_chunks_list, size_words);
    }

    assert(first_free_chunk_idx != (size_t) -1);

    pog_chunk first_free_chunk = freed_chunks_list.chunks[first_free_chunk_idx];
    if (first_free_chunk.size > size_words) {
        //remove old chunk
        pog_chunk_remove(&freed_chunks_list, first_free_chunk_idx);

        //add new, smaller chunk to freed chunks
        pog_chunk new_chunk = (pog_chunk) {
            .size = first_free_chunk.size - size_words,
            .start = first_free_chunk.start + size_words
        };
        pog_chunk_insert(&freed_chunks_list, new_chunk);

        //add chunk to alloced chunks
        pog_chunk alloced_chunk = (pog_chunk) {
            .size = size_words,
            .start = first_free_chunk.start
        };
        pog_chunk_insert(&alloced_chunks_list, alloced_chunk);

        return alloced_chunk.start;
    } else {
        //remove chunk from freed chunks
        pog_chunk_remove(&freed_chunks_list, first_free_chunk_idx);

        //insert chunk into alloced
        pog_chunk_insert(&alloced_chunks_list, first_free_chunk);

        return first_free_chunk.start;
    }
}

void pog_free(void *ptr) {
    //get the memory chunk of the ptr
    size_t idx = pog_chunk_by_ptr(&alloced_chunks_list, ptr);
    assert(idx != (size_t) -1);

    pog_chunk freed_chunk = alloced_chunks_list.chunks[idx];

    for (size_t i = 0; i < freed_chunk.size; ++i) {
        freed_chunk.start[i] = 0;
    }

    //move the memory chunk to the freed list
    pog_chunk_remove(&alloced_chunks_list, idx);
    pog_chunk_insert(&freed_chunks_list, freed_chunk);
}

void pog_squash() {
    pog_chunk_squash(&tmp_chunks_list, &freed_chunks_list);
    freed_chunks_list = tmp_chunks_list;
}

void* pog_realloc(void* ptr, size_t size_bytes) {
    size_t idx = pog_chunk_by_ptr(&alloced_chunks_list, ptr);
    assert(idx != (size_t) -1);

    const size_t size_words = (size_bytes + (sizeof(uintptr_t) - 1)) / sizeof(uintptr_t);

    if (size_words == alloced_chunks_list.chunks[idx].size) {
        return ptr;
    }

    size_t copy_amount = size_words > alloced_chunks_list.chunks[idx].size ?
            alloced_chunks_list.chunks[idx].size * (size_t) sizeof(uintptr_t) :
            size_bytes;

    void* new_ptr = pog_malloc(size_bytes);
    memcpy(new_ptr, alloced_chunks_list.chunks[idx].start, copy_amount);
    pog_free(ptr);
    return new_ptr;
}

#if FEATURE_DEBUG
void pog_debug() {
    printf("--------------------------------------------\n");
    pog_chunk_debug(alloced_chunks_list, "Alloced");
    printf("--------------------------------------------\n");
    pog_chunk_debug(freed_chunks_list, "Freed");
    printf("--------------------------------------------\n");
}
#else
void pog_debug() {
    assert(0 && "FEATURE_DEBUG is not enabled");
}
#endif

#if FEATURE_GC
void pog_gc_mark_region(const uintptr_t* start, const uintptr_t* end) {
    for (;start < end; start += 1) {
        const uintptr_t *p = (const uintptr_t *) *start;
        for (size_t i = 0; i < alloced_chunks_list.curr_size; ++i) {
            pog_chunk chunk = alloced_chunks_list.chunks[i];
            if (chunk.start <= p && p < chunk.start + chunk.size) {
                size_t idx = pog_chunk_by_ptr(&tmp_chunks_list, chunk.start);
                if (idx != (size_t) -1) {
                    pog_chunk_remove(&tmp_chunks_list, idx);
                    pog_gc_mark_region(chunk.start, chunk.start + chunk.size);
                }
            }
        }
    }
}

void pog_gc_collect() {
    const uintptr_t *stack_start = (const uintptr_t*)__builtin_frame_address(0);

    for (size_t i = 0; i < alloced_chunks_list.curr_size; i++) {
        pog_chunk_insert(&tmp_chunks_list, alloced_chunks_list.chunks[i]);
    }

    //mark unused pointers on the heap and stack
    pog_gc_mark_region(stack_start, stack_base + 1);

    //TODO: mark unused pointers in static memory

    for (size_t i = 0; i < tmp_chunks_list.curr_size; i++) {
        pog_free(tmp_chunks_list.chunks[i].start);
    }

    pog_squash();
}
#else
void pog_gc_collect() {
    assert(0 && "FEATURE_GC is not enabled");
}
#endif