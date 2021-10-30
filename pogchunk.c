#include "pogmalloc.h"

size_t pog_chunk_first_free(pog_chunk_list* list, size_t size_words) {
    for (int i = 0; i < list->curr_size; ++i) {
        if (list->chunks[i].size >= size_words) {
            return i;
        }
    }

    return -1;
}

void pog_chunk_insert(pog_chunk_list* list, pog_chunk to_insert) {
    assert(list->curr_size + 1 < list->max_size);
    list->chunks[list->curr_size] = to_insert;

    //sort chunks list
    for (size_t i = list->curr_size;
         i > 0 && list->chunks[i].start < list->chunks[i - 1].start;
         --i) {
        const pog_chunk t = list->chunks[i];
        list->chunks[i] = list->chunks[i - 1];
        list->chunks[i - 1] = t;
    }

    list->curr_size += 1;
}

void pog_chunk_remove(pog_chunk_list* list, size_t index) {
    assert(index < list->curr_size);
    for (size_t i = index; i < list->curr_size - 1; ++i) {
        list->chunks[i] = list->chunks[i + 1];
    }
    list->curr_size -= 1;
}

void pog_chunk_compress(pog_chunk_list* chunks) {

}