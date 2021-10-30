#include "pogmalloc.h"

void pog_init(uintptr_t* heap_start, size_t heap_size,
              pog_chunk* alloced_chunks_start, size_t alloced_chunks_size,
              pog_chunk* freed_chunks_start, size_t freed_chunks_size,
              int (*expand_function)(size_t words)) {
    metadata = &(pog_metadata){
            .start = heap_start,
            .end = (uintptr_t *) heap_start[heap_size - 1],
            .expand_function = expand_function
    };

    alloced_chunks = alloced_chunks_start;
    alloced_max_size = alloced_chunks_size;

    freed_chunks = freed_chunks_start;
    freed_max_size = freed_chunks_size;

    freed_chunks[freed_curr_size++] = (pog_chunk){
        .start = heap_start,
        .size = heap_size
    };
}

void *pog_malloc(size_t size_bytes) {
    if (size_bytes == 0) {
        return NULL;
    }

    const size_t size_words = (size_bytes + (sizeof(uintptr_t) - 1)) / sizeof(uintptr_t);

    pog_chunk* best_fit_chunk = pog_chunk_first_free(size_words, freed_chunks, freed_curr_size);

    if (best_fit_chunk->start == NULL) {
        //If no best fit was found, try to compress freed chunks and try again
        pog_chunk_compress(freed_chunks, &freed_curr_size, freed_max_size);
        best_fit_chunk = pog_chunk_first_free(size_words, freed_chunks, freed_curr_size);
    }

    if (best_fit_chunk->start == NULL) {
        //If still no chunk was found, expand the heap space
        if(!metadata->expand_function(size_words)) {
            //Something went wrong while expanding the heap space :(
            exit(1);
        }

        //If the expansion was successful, we can also expand the max size
        //(we expect the user to also expand the alloced and freed array)
        alloced_max_size += size_words;
        freed_max_size += size_words;

        best_fit_chunk = pog_chunk_first_free(size_words, freed_chunks, freed_curr_size);
    }

    assert(best_fit_chunk->start != NULL);

    //TODO

    return NULL;
}

void pog_free(void *ptr) {

}
