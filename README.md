# dlmalloc

This is a version of the douglea allocator modified to compile on every platform under the sun, while providing an abstracted set of callbacks for mmap, direct_mmap, munmap and mremap.
You do so by providing a `dl_mmap_functions` struct to `dl_create_mspace` with the appropriate callbacks to provide dlmalloc with a proper set of functions to allocate pages from the OS.

For the original go here:
https://gee.cs.oswego.edu/dl/html/malloc.html 
