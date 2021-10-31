# pogmalloc

A poggers malloc implementation

### Supports

* Static allocator
* Real heap allocator (via `sbrk(2)`)

### Credits

Credits to Tsoding and his amazing malloc series, this allocator was heavily inspired by his:
* https://github.com/tsoding/memalloc
* https://www.youtube.com/watch?v=sZ8GJ1TiMdk&ab_channel=TsodingDaily

#### Differences to memalloc

While pogmalloc was inspired by Tsodings memalloc, there are a couple of key differences. memalloc focuses on garbage collection, while 
pogmalloc aims to be as dynamic as possible, providing just a barebones malloc algorithm with specific memory model implementations built 
on top. Furthermore, pogmalloc actually can allocate memory on the actual heap (thus is not limited to static data) and, from what I've gathered
in my quick (and truth be told probably very inaccurate) testing, is faster than memalloc (but is also way less memory efficient than memalloc because 
chunks are only squashed as a method of last resort).
