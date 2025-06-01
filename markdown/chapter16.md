# 16. The Page Cache and Page Writeback

## Page cache - Why do we need it?

- Disk I/O is expensive.
- To minimize disk I/O, the kernel implements a disk cache, that stores data into physical memory instead of doing I/O requests.


## Page cache - approach to caching

- The page cache consists of physical pages in RAM.
    - It's contents correspond to a physical block on a disk.
- The page cache is dynamic, as it can:
    - grow to consume free memory
    - shrink te relieve memory pressure.

- We call the device that's being cached as the **backing store**.

## Page Cache - read caching

- Whenever the kernel begins a read operation, for example, the process issues a `read()` system call.
    - The kernel will check if the data is stored in the page cache.
        - If it is, it can simply read the data from it.
        - If it's not, it's a **cache miss**. In this case, the kernel needs to schedule block I/O operations to read the data from the disk, then populate the page cache with it.

## Page Cache - write caching

- Write cache's are implemented in three ways:
    - no-write
    - write-through cache
    - write-back

### no-write
- This approach doesn't do write caching, instead just invalidating the read cache and rereading the file.

### Write-through cache

- This approach, it'll update both the cache and file on disk
    - This approach avoids having to invalidate the read cache.

### Write-back

- This approach, the process will write to the page-cache.
- The backing-store (the disk containing the file) will not be updated immediately.
    - instead, the cache will we marked dirty, and will be added to a dirty list, where it'll be periodically written to the disk.
    - Once the disk is written to, the mark the cache as **clean**.

- The act of writing to the disk periodically is known as **writeback**.


## Cache Eviction

- There will be times where we'll have to make room for more relevant cache entries or to shrink the cache.
    - This process is known as **cache eviction**.

- The linux cache eviction simply replaces a clean page with something else.

- This brings up a question, which cache do we get rid of?

- There are a few popular strategies:
    - Least Recently Used (LRU)
    - Two-List Strategy

## File Cache Eviction - LRU

- LRU is least recently used.

- Here, the cache will keep track of when caches we're accessed.
- If a cache needs to remove one, it'll be the cache with the oldest timestamp.

- Problems with this approach is if a process opens multiple files.
    - Opening the file will timestamp the cache, but the more you open, the more you'll have to evict.


## File cache eviction - Two-List Strategy

- Due to LRUs problem with the process opening multiple files, Linux uses a modified version of LRU, known as the two-list approach.

- Here, linux will maintain two lists:
    - an active list
    - an inactive list.

- Files on the active list are not available for eviction.
- Files on the inactive list are ready for cache eviction.

- Files on the inactive list are promoted to the active list when they're used.

- The two-list strategy will actively try to balance the two lists.
- These lists are **queues**, so it'll dequeue the queues to balance it out.