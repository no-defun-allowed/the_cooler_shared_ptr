# The Cooler `shared_ptr`

`crc::cooler_shared_ptr` provides a [coalescing](https://sites.cs.ucsb.edu/~ckrintz/racelab/gc/papers/levanoni-on-the-fly-rc.pdf)
reference-counting smart pointer, which trades off immediate destruction for
fewer refcount updates. You may also call `crc::collect()` to cause collection to
run immediately.

This isn't thread-safe at all. I'm not entirely convinced it works with 
one thread, even. But RAII does suffice to provide a shadow stack of sorts.
