# 12. Memory Management

## Pages

- The kernel treats physical pages as a basic unit of memory management.
    - Although the processor's smallest addresable unit is a byte/word, the MMU deals with pages rather than bytes.
- With virtual memory, pages are the smallest unit worked with.
- Page sizes are architecture dependent.
    - Pages are 4 KB on 32 bit systems, and 8 KB on 64 bit architectures.

- The kernel represents pages with a `stuct page` type, defined in `<linux/mm_types.h>`
    - This structure is used to keep track of all the pages on the system, as the kernel needs to know if it's free, or who owns the page when it's not free.
```c
struct page{
    unsigned long flags;
    atomic_t _count;
    atomic_t _mapcount;
    unsigned long private;
    struct address_space* mapping;
    pgoff_t index;
    struct list_head lru;
    void* virtual;
}
```
- The flags field stores the staus of the page, where the flags are defined in `<linux/page-flags.h>`
- `_count` defines the usage count of the page, or how many references are pointing to the page.
    - If it's -1, no one's using the page, and it becomes available for use.
        - you should use `page_count()` to check if a page is free.
- The virtual pointer holds the address of the page in virtual emory.



## Zones

- The kernel can't treat pages as identical, because some of the physical addresses in memory cannot be used for certain tasks
    - There's memory mapped I/O, which isn't memory

- The kernel uses zones to group pages based on their properties:
    - `ZONE_DMA`: This zone contains pages where devices can perform Direct Memory Access: Typically `< 16 mb`
    - `ZONE_DMA32`: These zones can also undergo DMA, but it's accessible only to 32 bit addresses. 
    - `ZONE_NORMAL` contains normal, regurlarly mapped pages. typically `16-896 mb`
    - `ZONE_HIGHMEM` contains "high memory" which are pages not mapped into the kernel's address space. typically `> 896 mb`


- Zones group pages.

## Getting Pages

- The kernel provides a low level mechanism for requesting memory.

```c
struct page* alloc_page(gfp_t gfp_mask, unsigned int order);
```
- This function 2^order contiguous physical pages, and returns a pointer to the first page's page structure.
    - If it fails, it returns NULL.

```c
void* page_address(struct page* page);
```

- this function will return the logical address of a given page.

```c
unsigned long __get_free_pages(gfp_t gfp_mask, unsigned int order);
```

- This function is siilar to `alloc_pages()`, but it directly returns the logical address of the first requested page.

```c
unsigned long get_zeroed_page(unsigned int gfp_mask)
```

- This allocates a single page, and sets all the bytes to zero.

### freeing pages

```c
void __free_pages(struct page* page, unsigned int order);
void free_pages(unsigned long addr, unsigned int order);
void free_page(unsigned long addr);
```

- Similar to malloc, you don't want to double free, or free invalid pages.

## kmalloc/kfree

- `kmalloc()` is the kernel space version of malloc

```c
void* kmalloc(size_t size, gfp_t flags);
```

- You should also handle errors.

- `kfree()` is the exact same as `free()` in userspace.


## vmalloc

- vmalloc is similar to `kmalloc()`, but it allocates memory that's only virtually contiguous, and not physically contiguous.
- Only hardware will need physically contiguous memory allocation.
    - Despite this, most kernel code uses `kmalloc()` instead for performance, as having separate pages would caes poor locality.
## gfp_mask flags

- When we allocate pages using `alloc_pages`, we need to pass in flags, which are gfp .

- gfp stands for get_free_pages, which is one of the memory allocation functions.

- We can break flags into 3 categories:
    - `action modifiers`: Specifies how kernel is supposed to allocate the requested memory
    - `zone modifiers`: where to allocate memory.
    - `types`: combines action and zone modifiers into one flag.
        - for example `GFP_KERNEL` is a type flag that is used for code in the process context inside the kernel.
- There are a lot of flags

```c
ptr = kmalloc(size, __GFP_WAIT | __GPF_IO | __GFP_FS);
```

## Slab Layer

- The most common operation a kernel does is allocate and free data structures.
    - To facilitate frequent allocations and deallocations of data, programmers often use a free list.
        - This provides a lost of blocks of available, already allocated data structures.
        - Once the data structure is no longer needed, it's returned to the free list instead of deallocated.
        - This approach is fast, but has problems

- The caching approach has an issue, there's no global control:
    - If memory is low, the kernel can't communicate to every free list that it should shrink the size.
- this issue gave birth to the **slab allocator**

### What is the slab allocator

- The slab layer divides different objects into groups called caches.
    - Each group stores a different type of object.

- These caches are broken down into slabs.
    - The slab consists of one or more physically contiguous pages.

- Each slab will contain a number of object, which have data structures being cached.
- Each slab contains a number of objects, which are the data structures being cached.
    - Each slab have three states:
        - full, partial and empty.
- When the kernel wants a data structure, it'll check the slab.
    - If there's a partial slab, it'll just take one of the structs.
    - If there's no partial slab, create a new one.
    - If the slab's full, create a new slab.

## Creating/destroying slabs

- slab's are created with the `kmem_cache_create` function

```c
struct kmem_cache* kmem_cache_create(const char* name, size_t size, size_t align, unsigned long flags, void (*ctor)(void*));
```

 - name is a string containing the name of the cache.
 - Second argument is the size of each element in the cache.
- Third parameter is the offset of the first object, to ensure a particular alignment within the page.
- flags specify optional settings
- ctor is a constructor function for the cache. You can use NULL if you want.
- This function returns NULL if it fails

### Destroying

- slabs are destroyed using the `kmem_cache_destroy()` function.

```c
int kmem_cache_destroy(struct kmem_cache* cachep);
```
- You pass in the struct created by the create function, and it deallocates the slab.

## Allocating/Freeing from the cache slab

```c
void* kmem_cache_alloc(struct kmem_cache* cachep, gfp_t flags);
```
- This retrieves the object from the slab. the function will handle the resizing of the slab if there's not enough space.

### Freeing

```c
void kmem_cache_free(struct kmem_cache* cachep, void* objp);
```
- cachep is the slab created.
- objp is the object that's allocated. 

## Kernel Stack

- Unlike userspace, the kernel stack is fixed in size.
    - Historically, the kernel stack would be 2 pages (8kb on 32 bit, 16kb on 64 bit).

- In newer versions of the kernel, the size is 1 page.
- Interrupt handlers used to use the kernel stack, but then a special interrupt stack was introduced to save space.


When it comes to using the kernel stack, you shouldn't allocate large types on the stack, as that could potentially cause stack overflow, which can corrupt your system.


## Low vs. High memory

- **low memory**: Low memory refers to physical memory that is permanently mapped into the kernel’s virtual address space.
- **high memory**:  High memory is physical RAM that is not permanently mapped into the kernel's virtual address space.

## High Memory Mapping

- Because high memory pages aren't permanent, we need to explicitly map them into the kernel address space.
    - this is done with `kmap`

```c
void* kmap(struct page* page);
```

- This function will map a page into the kernel address space.
- This function does sleep, so it'll only work in process context.

```c
kunmap(struct page* page);
```
- Because there are a limited number of pages, you need to unmap the page using `kunmap()`

## Temporary high address mapping

- To map pages into kernel space in interrupt context, you can use `kmap_atomic()` instead.

```c
void* kmap_atomic(struct page* page, enum km_type type);
```
- The type parameter is a lot of enums.

```c
void kunmap_atomic(void* kvaddr, enum km_type type);
```
- This frees the page at the address kvaddr.
    - kvaddr is the address returned by kmap_atomic.

## Per-CPU Allocations

- With multicore processors, each CPU will have it's own data:
    - Kernel stack, interrupt stuff, etc.
- the per-CPU data is stored into an array.
    - Each item on the array corresponds to a possibel processor on the system.

```
unsigned long my_percpu[NR_CPUS];
```
- We can represent the different processors as an array of cpus.

```c
int cpu; 
cpu = get_cpu(); /* disables kernel preemption */
my_percpu[cpu]++;
printk("cpu %d is %li\n",cpu,my_percpu[cpu]);
put_cpu(); /* reenables kernel preemption */
```

- Because the data is unique to the processor, there's no need to use locks.

## new percpu interface, defining at compile vs. runtime

- The linux kernel version 2.6 introduces a new interface called percpu, which lets you create and manipulate per-CPU data.

### defining CPU data at compile time

```c
DEFINE_PER_CPU(type,name); /* declare per processor variables */

get_cpu_var(name)++; /* increment name on the processor */
put_cpu_var(name); /* reenable kernel preemption */

per_cpu(name, cpu)++; /* increment name on another CPU (YOU NEED TO USE LOCKS WITH THIS ONE)*/
```

### defining CPU data at runtime, `__alignof__` feature

```c
void *alloc_percpu(type); /* macro that simplifies things */
void* __alloc_percpu(size_t size, size_t align); /* similar to alloc_percpu, but more complicated*/
void free_percpu(const void*); /* frees memory */


/* example */
struct rabid_cheetah* ptr = alloc_percpu(struct rabid_cheetah);

struct rabid_cheetah* ptr = __alloc_percpu(sizeof(struct rabid_cheetah), __alignof__ (struct rabid_cheetah));

/* using our data */
get_cpu_var(ptr);
put_cpu_var(ptr);
```
- These two do the same thing, 
- `__alignof__` adds additional alignment for the type, which varies by architecture.

## Why use per-cpu data

- Per-CPU data is easier to work with, since there's a reduction in locking requirements.
- It's also more cache friendly, since other cores can't invalidate the cache of your data. 

