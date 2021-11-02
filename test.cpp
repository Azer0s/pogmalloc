#include <catch2/catch_test_macros.hpp>

extern "C" {
#include "pog_static_heap.h"
}

TEST_CASE("Squash freed memory", "[factorial]" ) {
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