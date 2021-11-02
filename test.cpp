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

TEST_CASE("GC unused pointers") {
    //TODO
}

TEST_CASE("GC stack pointers that have been set to null") {
    //TODO
}

TEST_CASE("GC static pointers that have been set to null") {
    //TODO
}