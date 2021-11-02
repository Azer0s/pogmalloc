cmake_minimum_required(VERSION 3.20)
project(pogmalloc C)

set(CMAKE_C_STANDARD 11)

add_executable(pogmalloc main.c pogmalloc.c pogchunk.c)
target_compile_definitions(pogmalloc PUBLIC -DFEATURE_DEBUG -DFEATURE_GC)

enable_language(CXX)
Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.0-preview3
)
FetchContent_MakeAvailable(Catch2)
add_executable(pogmalloc_test test.cpp pogmalloc.c pogchunk.c)
target_link_libraries(pogmalloc_test PRIVATE Catch2::Catch2WithMain)
target_compile_definitions(pogmalloc_test PUBLIC -DFEATURE_GC)

