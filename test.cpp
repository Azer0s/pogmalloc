#include <catch2/catch_test_macros.hpp>

extern "C" {
#include "pog_static_heap.h"
}

TEST_CASE("Malloc 0 bytes") {
    pog_static_heap_init();

    REQUIRE(pog_malloc(0) == NULL);
}

static auto was_called = false;

TEST_CASE("Allocate static memory until the expand function is called") {
    uintptr_t test_heap[3] = {0};
    pog_chunk test_alloced[3] = {nullptr};
    pog_chunk test_freed[3] = {nullptr};
    pog_chunk test_tmp[3] = {nullptr};

    pog_init(&test_heap[0], 2,
             &test_alloced[0], 2,
             &test_freed[0], 2,
             &test_tmp[0], 2,
             [](size_t words, size_t* alloced_chunks_size, size_t* freed_chunks_size) -> int {
                 REQUIRE(words == (size_t) 1);
                 (*alloced_chunks_size)++;
                 (*freed_chunks_size)++;

                 was_called = true;

                 return 0;
             });

    void* a = pog_malloc(1);
    REQUIRE(test_alloced[0].size == (size_t) 1);
    REQUIRE(test_freed[0].size == (size_t) 1);

    void* b = pog_malloc(1);
    REQUIRE(test_alloced[0].size == (size_t) 1);
    REQUIRE(test_alloced[1].size == (size_t) 1);

    void* c = pog_malloc(1);
    REQUIRE(test_alloced[0].size == (size_t) 1);
    REQUIRE(test_alloced[1].size == (size_t) 1);
    REQUIRE(test_alloced[1].size == (size_t) 1);

    REQUIRE(was_called);
}

TEST_CASE("Squash freed memory") {
    pog_static_heap_init();

    void* a = pog_malloc(1);
    void* b = pog_malloc(1);
    void* c = pog_malloc(1);

    REQUIRE(a != b);
    REQUIRE(b != c);
    REQUIRE(a != c);

    REQUIRE(freed[0].size == (size_t) 7997);

    pog_free(a);
    pog_free(b);
    pog_free(c);

    REQUIRE(freed[0].size == (size_t) 1);
    REQUIRE(freed[1].size == (size_t) 1);
    REQUIRE(freed[2].size == (size_t) 1);
    REQUIRE(freed[3].size == (size_t) 7997);

    pog_squash();

    REQUIRE(freed[0].size == (size_t) 8000);
}

TEST_CASE("Realloc allocated memory") {
    pog_static_heap_init();

    void* a = pog_malloc(8);

    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 7999);

    a = pog_realloc(a, 16);

    REQUIRE(alloced[0].size == (size_t) 2);
    REQUIRE(freed[0].size == (size_t) 1);
    REQUIRE(freed[1].size == (size_t) 7997);

    pog_squash();

    REQUIRE(freed[0].size == (size_t) 1);
    REQUIRE(freed[1].size == (size_t) 7997);
}

TEST_CASE("GC unused stack pointer") {
    pog_static_heap_init();
    pog_gc_init(NULL, 0);

    void* a = pog_malloc(1);
    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 7999);

    pog_gc_collect();
    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 7999);

    a = nullptr;
    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 7999);

    pog_gc_collect();
    REQUIRE(freed[0].size == (size_t) 1);
    REQUIRE(freed[1].size == (size_t) 7999);

    pog_squash();
    REQUIRE(freed[0].size == (size_t) 8000);
}

TEST_CASE("GC multiple unused stack pointers") {
    pog_static_heap_init();
    pog_gc_init(NULL, 0);

    void* a = pog_malloc(1);
    void* b = pog_malloc(1);
    void* c = pog_malloc(1);
    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(alloced[1].size == (size_t) 1);
    REQUIRE(alloced[2].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 7997);

    pog_gc_collect();
    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(alloced[1].size == (size_t) 1);
    REQUIRE(alloced[2].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 7997);

    a = nullptr;
    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 7997);

    pog_gc_collect();
    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(alloced[1].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 1);
    REQUIRE(freed[1].size == (size_t) 7997);

    b = nullptr;
    c = nullptr;
    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(alloced[1].size == (size_t) 1);

    pog_gc_collect();
    REQUIRE(freed[0].size == (size_t) 1);
    REQUIRE(freed[1].size == (size_t) 1);
    REQUIRE(freed[2].size == (size_t) 1);
    REQUIRE(freed[3].size == (size_t) 7997);

    pog_squash();
    REQUIRE(freed[0].size == (size_t) 8000);
}

TEST_CASE("GC pointers in heap without GC root") {
    pog_static_heap_init();
    pog_gc_init(NULL, 0);

    struct LinkedList {
        int val;
        LinkedList* next;
    };
    auto a = (LinkedList*) pog_malloc(sizeof(LinkedList));
    auto b = (LinkedList*) pog_malloc(sizeof(LinkedList));
    auto c = (LinkedList*) pog_malloc(sizeof(LinkedList));
    auto d = (LinkedList*) pog_malloc(sizeof(LinkedList));

    a->val = 1;
    a->next = b;

    b->val = 2;
    b->next = c;

    c->val = 3;
    c->next = d;

    d->val = 4;

    REQUIRE(alloced[0].size == (size_t) 2);
    REQUIRE(alloced[1].size == (size_t) 2);
    REQUIRE(alloced[2].size == (size_t) 2);
    REQUIRE(alloced[3].size == (size_t) 2);
    REQUIRE(freed[0].size == (size_t) 7992);

    pog_gc_collect();
    REQUIRE(alloced[0].size == (size_t) 2);
    REQUIRE(alloced[1].size == (size_t) 2);
    REQUIRE(alloced[2].size == (size_t) 2);
    REQUIRE(alloced[3].size == (size_t) 2);
    REQUIRE(freed[0].size == (size_t) 7992);

    b->next = nullptr;
    c = nullptr;
    d = nullptr;

    pog_gc_collect();
    REQUIRE(alloced[0].size == (size_t) 2);
    REQUIRE(alloced[1].size == (size_t) 2);
    REQUIRE(freed[0].size == (size_t) 2);
    REQUIRE(freed[1].size == (size_t) 2);
    REQUIRE(freed[2].size == (size_t) 7992);

    pog_squash();
    REQUIRE(alloced[0].size == (size_t) 2);
    REQUIRE(alloced[1].size == (size_t) 2);
    REQUIRE(freed[0].size == (size_t) 7996);

    a = nullptr;
    b = nullptr;
    pog_gc_collect();
    REQUIRE(freed[0].size == (size_t) 2);
    REQUIRE(freed[1].size == (size_t) 2);
    REQUIRE(freed[2].size == (size_t) 7996);

    pog_squash();
    REQUIRE(freed[0].size == (size_t) 8000);
}

static char* x;
static char* y;
static char* z;

#define STATIC_MEM_CAP 2048
uintptr_t test_static_mem[STATIC_MEM_CAP];

TEST_CASE("GC static pointers that have been set to null") {
    pog_gc_init(&test_static_mem, STATIC_MEM_CAP);
    pog_gc_mark_static(&x);
    pog_gc_mark_static(&y);
    pog_gc_mark_static(&z);

    pog_static_heap_init();

    x = (char*) pog_malloc(1);
    y = (char*) pog_malloc(1);
    z = (char*) pog_malloc(1);

    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(alloced[1].size == (size_t) 1);
    REQUIRE(alloced[2].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 7997);

    pog_gc_collect();
    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(alloced[1].size == (size_t) 1);
    REQUIRE(alloced[2].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 7997);

    x = nullptr;
    pog_gc_collect();
    REQUIRE(alloced[0].size == (size_t) 1);
    REQUIRE(alloced[1].size == (size_t) 1);
    REQUIRE(freed[0].size == (size_t) 1);
    REQUIRE(freed[1].size == (size_t) 7997);

    y = nullptr;
    z = nullptr;
    pog_gc_collect();
    REQUIRE(freed[0].size == (size_t) 1);
    REQUIRE(freed[1].size == (size_t) 1);
    REQUIRE(freed[2].size == (size_t) 1);
    REQUIRE(freed[3].size == (size_t) 7997);

    pog_squash();
    REQUIRE(freed[0].size == (size_t) 8000);
}
