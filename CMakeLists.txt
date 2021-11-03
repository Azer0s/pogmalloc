cmake_minimum_required(VERSION 3.20)
project(pogmalloc C)

set(CMAKE_C_STANDARD 11)

# Define library
include(GNUInstallDirs)
add_library(pogmalloc pogmalloc.c pogchunk.c)
set_target_properties(pogmalloc PROPERTIES
        VERSION 1.0
        SOVERSION 1
        PUBLIC_HEADER pogmalloc.h)
target_include_directories(pogmalloc PRIVATE .)

# Define executable (for quick dirty testing during development)
add_executable(pogmalloc_exe main.c pogmalloc.c pogchunk.c)
target_compile_definitions(pogmalloc_exe PUBLIC -DFEATURE_DEBUG -DFEATURE_GC)

# Include Catch2
enable_language(CXX)
Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.0-preview3
)
FetchContent_MakeAvailable(Catch2)

# Define test project
add_executable(pogmalloc_test test.cpp pogmalloc.c pogchunk.c)
target_link_libraries(pogmalloc_test PRIVATE Catch2::Catch2WithMain)
target_compile_definitions(pogmalloc_test PUBLIC -DFEATURE_GC)

