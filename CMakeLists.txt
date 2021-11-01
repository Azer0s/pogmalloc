cmake_minimum_required(VERSION 3.20)
project(pogmalloc C)

set(CMAKE_C_STANDARD 11)

add_definitions(-DFEATURE_DEBUG)
add_definitions(-DFEATURE_GC)

add_executable(pogmalloc main.c pogmalloc.c pogchunk.c)
