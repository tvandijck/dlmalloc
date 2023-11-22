/*
  Modified to support Virtual Memory Tracking.
*/

/*
  Default header file for malloc-2.8.x, written by Doug Lea
  and released to the public domain, as explained at
  http://creativecommons.org/publicdomain/zero/1.0/

  This header is for ANSI C/C++ only.  You can set any of
  the following #defines before including:

  * If USE_DL_PREFIX is defined, it is assumed that malloc.c
    was also compiled with this option, so all routines
    have names starting with "dl".

  * If HAVE_USR_INCLUDE_MALLOC_H is defined, it is assumed that this
    file will be #included AFTER <malloc.h>. This is needed only if
    your system defines a struct mallinfo that is incompatible with the
    standard one declared here.  Otherwise, you can include this file
    INSTEAD of your system system <malloc.h>.  At least on ANSI, all
    declarations should be compatible with system versions

  * If MSPACES is defined, declarations for dl_mspace versions are included.
*/

#ifndef MALLOC_280_H
#define MALLOC_280_H

#include <stddef.h>   /* for size_t */

struct dl_mallinfo {
    size_t arena;    /* non-mmapped space allocated from system */
    size_t ordblks;  /* number of free chunks */
    size_t smblks;   /* always 0 */
    size_t hblks;    /* always 0 */
    size_t hblkhd;   /* space in mmapped regions */
    size_t usmblks;  /* maximum total allocated space */
    size_t fsmblks;  /* always 0 */
    size_t uordblks; /* total allocated space */
    size_t fordblks; /* total free space */
    size_t keepcost; /* releasable (via malloc_trim) space */
};

/*
  dl_mspace is an opaque type representing an independent
  region of space that supports mspace_malloc, etc.
*/
typedef void* dl_mspace;

/*
  mmap_functions is a used for callbacks to the system
  you must provide it to create_mspace function.
*/
struct dl_mmap_functions {
    void* (*mmap)(size_t* size, void* context);
    void* (*direct_mmap)(size_t* size, void* context);
    int   (*munmap)(void* ptr, size_t size, void* context);
    void* (*mremap)(void* ptr, size_t size, size_t newsize, int flags, void* context);
    void* context;
};

/*
  create_mspace creates and returns a new independent space with the
  given initial capacity, or, if 0, the default granularity size.  It
  returns null if there is no system memory available to create the
  space.  If argument locked is non-zero, the space uses a separate
  lock to control access. The capacity of the space will grow
  dynamically as needed to service mspace_malloc requests.  You can
  control the sizes of incremental increases of this space by
  compiling with a different DEFAULT_GRANULARITY or dynamically
  setting with mallopt(M_GRANULARITY, value).
*/
dl_mspace dl_create_mspace(size_t capacity, int locked, struct dl_mmap_functions* funcs);

/*
  destroy_mspace destroys the given space, and attempts to return all
  of its memory back to the system, returning the total number of
  bytes freed. After destruction, the results of access to all memory
  used by the space become undefined.
*/
size_t dl_destroy_mspace(dl_mspace msp);

/*
  mspace_track_large_chunks controls whether requests for large chunks
  are allocated in their own untracked mmapped regions, separate from
  others in this dl_mspace. By default large chunks are not tracked,
  which reduces fragmentation. However, such chunks are not
  necessarily released to the system upon destroy_mspace.  Enabling
  tracking by setting to true may increase fragmentation, but avoids
  leakage when relying on destroy_mspace to release all memory
  allocated using this space.  The function returns the previous
  setting.
*/
int dl_mspace_track_large_chunks(dl_mspace msp, int enable);

/*
  mspace_mallinfo behaves as mallinfo, but reports properties of
  the given space.
*/
struct dl_mallinfo dl_mspace_mallinfo(dl_mspace msp);

/*
  An alias for mallopt.
*/
int dl_mspace_mallopt(int, int);

/*
  The following operate identically to their malloc counterparts
  but operate only for the given dl_mspace argument
*/
void*  dl_mspace_malloc(dl_mspace msp, size_t bytes);
void   dl_mspace_free(dl_mspace msp, void* mem);
void*  dl_mspace_calloc(dl_mspace msp, size_t n_elements, size_t elem_size);
void*  dl_mspace_realloc(dl_mspace msp, void* mem, size_t newsize);
void*  dl_mspace_realloc_in_place(dl_mspace msp, void* mem, size_t newsize);
void*  dl_mspace_memalign(dl_mspace msp, size_t alignment, size_t bytes);
void** dl_mspace_independent_calloc(dl_mspace msp, size_t n_elements, size_t elem_size, void* chunks[]);
void** dl_mspace_independent_comalloc(dl_mspace msp, size_t n_elements, size_t sizes[], void* chunks[]);
size_t dl_mspace_bulk_free(dl_mspace msp, void**, size_t n_elements);
size_t dl_mspace_usable_size(const void* mem);
void   dl_mspace_malloc_stats(dl_mspace msp);
int    dl_mspace_trim(dl_mspace msp, size_t pad);
size_t dl_mspace_footprint(dl_mspace msp);
size_t dl_mspace_max_footprint(dl_mspace msp);
size_t dl_mspace_footprint_limit(dl_mspace msp);
size_t dl_mspace_set_footprint_limit(dl_mspace msp, size_t bytes);

typedef void(*dl_inspect_handler)(void *start, void *end, size_t used_bytes, void* callback_arg);

void   dl_mspace_inspect_all(dl_mspace msp, dl_inspect_handler handler, void* arg);

#endif /* MALLOC_280_H */
