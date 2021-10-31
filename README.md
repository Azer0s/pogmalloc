# pogmalloc(3)

A poggers malloc implementation

### Features

* Static allocator
* Real heap allocator (via `sbrk(2)`)
* Works on ARM & x86_64
* Works with GCC and clang 

### Limitations

* No packed structs (tho, this isn't really tested yet)
* Doesn't work on Windows
* Kinda flimsy tbh (it's cool and all but don't use it in prod lmao)

### Credits

Credits to Tsoding and his amazing malloc series, this allocator was heavily inspired by his:
* https://github.com/tsoding/memalloc
* https://www.youtube.com/watch?v=sZ8GJ1TiMdk&ab_channel=TsodingDaily

#### Differences to memalloc

While pogmalloc was inspired by Tsodings memalloc, there are a couple of key differences. memalloc focuses on garbage collection, while 
pogmalloc aims to be as dynamic as possible, providing just a barebones malloc algorithm with specific memory model implementations built 
on top (I originally built pogmalloc as a bare metal OS allocator for an OSDev project). Furthermore, pogmalloc can allocate memory on the actual heap (thus is not limited to static data) and, from what I've gathered
in my quick (and truth be told, probably very inaccurate) testing, is faster than memalloc (but is also way less memory efficient than memalloc because 
chunks are only squashed as a method of last resort).
