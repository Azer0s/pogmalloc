#include "pogmalloc.h"

size_t pog_chunk_first_free(pog_chunk_list* list, size_t size_words) {
    for (size_t i = 0; i < list->curr_size; ++i) {
        if (list->chunks[i].size >= size_words) {
            return i;
        }
    }

    return -1;
}

size_t pog_chunk_by_ptr(pog_chunk_list *list, void *ptr) {
    for (size_t i = 0; i < list->curr_size; ++i) {
        if (list->chunks[i].start == ptr) {
            return i;
        }
    }

    return -1;
}

void pog_chunk_insert(pog_chunk_list* list, pog_chunk to_insert) {
    assert(list->curr_size < list->max_size);
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

void pog_chunk_squash(pog_chunk_list* dst, pog_chunk_list* src) {
    dst->curr_size = 0;

    for (size_t i = 0; i < src->curr_size; ++i) {
        if (dst->curr_size > 0) {
            if (dst->chunks[dst->curr_size - 1].start + dst->chunks[dst->curr_size - 1].size == src->chunks[i].start) {
                (&dst->chunks[dst->curr_size - 1])->size += src->chunks[i].size;
            } else {
                pog_chunk_insert(dst, src->chunks[i]);
            }
        } else {
            pog_chunk_insert(dst, src->chunks[i]);
        }
    }
}

#if FEATURE_DEBUG
void pog_chunk_debug(pog_chunk_list list, const char *name) {
    printf("%s Chunks (%zu):\n", name, list.curr_size);
    for (size_t i = 0; i < list.curr_size; ++i) {
        printf("  start: %p, size: %zu\n",
               (void*) list.chunks[i].start,
               list.chunks[i].size);
    }
}
#else
void pog_chunk_debug(pog_chunk_list list, const char *name) {
    assert(0 && "FEATURE_DEBUG is not enabled");
}
#endif
