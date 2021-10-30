#include "pogmalloc.h"

pog_chunk* pog_chunk_first_free(size_t size_words, pog_chunk* chunks, size_t chunks_count) {
    pog_chunk* best_fit_chunk = &(pog_chunk){
            .start = NULL,
            .size = SIZE_MAX
    };

    for (int i = 0; i < chunks_count; ++i) {
        if (chunks[i].size >= size_words && chunks[i].size < best_fit_chunk->size) {
            best_fit_chunk = &chunks[i];
        }
    }

    if (best_fit_chunk->start == NULL) {
        return NULL;
    }

    return best_fit_chunk;
}

void pog_chunk_insert(pog_chunk to_insert, pog_chunk* chunks, size_t* chunks_count, size_t chunks_max) {
    assert((*chunks_count) + 1 < chunks_max);
    chunks[*chunks_count] = to_insert;

    //sort chunk list
    for (size_t i = *chunks_count;
         i > 0 && chunks[i].start < chunks[i - 1].start;
         --i) {
        const pog_chunk t = chunks[i];
        chunks[i] = chunks[i - 1];
        chunks[i - 1] = t;
    }

    (*chunks_count) += 1;
}

void pog_chunk_compress(pog_chunk* chunks, size_t* chunks_count, size_t chunks_max) {

}