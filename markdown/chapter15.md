# 15. The Process Address Space


## Address Space - processes and threads

- The process address space consists of virtual memory addressable by a process.
    - Each process will only have access to the virtual memory allocated for the process.
- each process is given a flat 32 bit or 64 bit address sapce depending on the architecture
- Each process will have it's own address space, so two processes try to write to address 0xFFF, they won't be accessing the same memory address.

- Processes can opt to share their address space. These processes are known as threads

## Address Space - flat vs. segmented

- flat memory address means you have access to the entire range of memory addresses (example: 32 bits, you can access 0 to 4294967295)
- Segmented means you can only access particular segments of the address space.

## Address Space - permissions

- We uniquely identify memory in an address space with memory addresses.
- There are legal and illegal memory addresses:
    - legal addresses are called memory areas.
        - The kernel can add/remove memory areas to its address space.

- Memory areas have associated permissions:
    - read permissions
    - write permisisions
    - execute permissions
- attempting to access illegal areas, or accessing a memory area incorrectly will result in a "segmentation fault" message.

## Memory Areas

- memory areas are the regions of memory the process can access.
- It consists of these areas areas:
    - memory map of the executable file's code, called the text section
    - A memeory map of the executable file's initialized global variables, called the data position
    - Memory map of the zero page (page initialized to all zeroes), containing uninitialized global variables, known as the `bss` section
    - A memory map of the zero page is used for the process's user space stack.
    - additional  text, data and bss for shaed library
    - memory mapped files
    - shared memory segments
    - other mappings, like those associated with `malloc()`

## Memory Descriptors

- The kernel represents the process's address space with a structure known as the memory descriptor.
    - The memory descriptor is represented with the `mm_struct`

### mm_struct

- mm_struct is the memory descriptor struct representation.

```c
struct mm_struct{
    /* ... */
    atomic_t    mm_users;
    atomic_t    mm_count;
}
```

- the mm_users contains how many users have access to this address space.
    - If 9 threads are sharing the same address space, mm_users will be 9.
    - regardless of how many threads you have, `mm_count` will only increment once.
    - When mm_users becomes zero, `mm_count` decrements
- The `mm_count` variable is the primary counter. When it reaches 0, the kernel frees the address sapce.

## Allocating/Freeing Memory Descriptor

### Allocating Memory Descriptor

- You can get the current processes memory descriptor with the `current->mm` macro.
- you can clone a memory descriptor with `copy_mm()`
- memory can be allocated with the mm_cachep slab with the `allocate_mm()` macro in `kernel/fork.c`.
- processes can use `clone()` with their `CLONE_VM` flag to clone the address space.

### Destroying memory descriptor

- when a process exits with a specific address space, it will call `exit_mm()`, which will decrement mm_users user count.
    - Similar to before, if mm_users reaches 0, it'll decrement mm_count.
        - If that becomes 0, then free_mm() is invoked.
    
## Kernel Threads and mm field

- Kernel threads only run in kernel space (duh).
    - There's no such thing as a user space address space.
- Because of this, th mm field of the `task_struct` is NULL, since there's no user memory to manage.
- This saves time, since there's only, for example, one kernel stack.
    - When you run a kernel thread, it can use the previous running process.

- AKA, if the scheduler sees mm is NULL, it will keep the previous address space active.

## Virtual Memory Areas, feature of structure

- the memory area structure, `vm_area_struct`, represents memory areas.
    - in the kernel, memory areas are more ofter called virtual memory areas (VMAs).

- VMAs are described by the `vm_area_struct` over a contiguous interval in an address space.
- The kernel treats each memory area as a unique memory object.

### `vm_area_struct` features

```c
struct vm_area_struct{
    unsigned long                   vm_start;
    unsigned long                   vm_end;
    mm_struct*                      vm_mm;
    unsigned long                   vm_flags;
    struct vm_operations_struct*    vm_ops;
}
```

- `vm_start` and `vm_end` define the address range of the virtual address space.
- `vm_mm` is a pointer that keeps track of what mm_struct the vma is associated with.
- `vm_flags` lets us specify the behavior and information about the pages contained in the memory area.
    - We can set read, write, exec and shared permissions using:
        - `VM_READ`
        - `VM_WRITE`
        - `VM_EXEC`
        - `VM_SHARED`
    - There are more flags too.
- vm_ops is a struct containing pointers to operators
    - this includes things like open, close, fault, etc.


## Manipulating Memory Areas

- The kernel can perform operations on memory areas

```c
struct vm_area_struct* find_vma(struct mm_struct* mm, unsigned long address);
```
    - This function will return the memory area that contains the addr, or begins at an address greater than the address
        - If this doesn't exist, NULL is returned.

```c
struct vm_area_struct* find_vma_prev(truct mm_struct* mm, unsigned long addr, struct nm_area_struct** pprev);
```
- This will find the last VMA before addr.

## `mmap` and `do_mmap()`

- The `do_mmap` function is a kernel space function that creates a new linear address interval.
    - If the interval neighbors an existing address interval, they'll share the same virtual memory area.
    - If it can't find a neighboring address interval, it'll create a new VMA.

```c
unsigned long do_mmap(struct file* file, unsigned long addr, unsigned long len, unsigned long prot, unsigned long flag, unsigned long offset);
```

- You can map the file by using a file type, and setting the offset to whatever you want. The file is of length n.
- If you don't want to use a file, you can set file to NULl, and offset to 0.
- It allocates pages, you can't allocate bytes.
    - there's a `PAGE_SIZE` macro for convenience.
- The prot parameters define protections available:
    - `PROT_READ`
    - `PROT_WRITE`
    - `PROT_EXEC`
    - `PROT_NONE`
- flags aret he options:
    - `MAP_SHARE` lets processes share the same memory
    - `MAP_PRIVATE` means it cannot be shared.
    - There's a lot more.

- If do_mmap() fails, it'll return a negative value.

### `mmap`

- `mmap` is the user space version of do_mmap.

```c
void * mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
```
- len and offset are in bytes.
- Instead of using a file struct, you can use a file descriptor
- If you just want to allocate space, you use -1.

### Cleaning

- If you allocate an address space with `mmap()` and `munmap()`, you need to free it with `munmap()` or `do_munmap()`.

## `munmap()` and `do_munmap()`

- To remove an address interval from a process, you use `do_munmap()` or `munmap()`.

```c
int do_munmap(struct mm_struct* mm, unsigned long start, size_t len);
int munmap(void* start, size_t length);
```
- mm is a pointer to the current mm_struct. When you free the address space, it needs to change some metadata.
- start is the address the address space starts at.
- len specifies the length.

## Page tables, 3 layers, performance

- The Operating System, in user space, will take the virtual addresses, then convert them into the physical address.
- The OS does this through page tables.
    - It takes virtual addresses, and turn them into chunks.
        - Each chunk is used as an index into a page table.
        - The table then points to either another tabel or the physical page.

### 3 layers

- In linux, the page table has 3 layers, allowing for a sparsely populated address space.
    - The top level is the page global directory:
        - This is an array of `pgd_t` types. This is usually an unsigned long.
        - Each `pgd` points to entries in the second level directory
    - The second layer is the PMD, or the **page middle directory**
        - This is an array of `pmd_t` types, which point to entries in the PTE.
    - The final level is the `pte_t`, or the **page table entry**.
        - This points to the physical page in memory, which is what we want.

### performance

- Looking up the specific items in each layer costs performance.
- To facilitate this, processors implement a **translation lookaside buffer**, TLB for short.
    - This acts as a hardware cache of virtual to physical mappings.


## Page Table - Translation of page addresses

- Using page tables, we break the memory address into two parts:
    - the page number (top y bits)
        - on 32 bit sytems, since the page offset is 12 bits, the page number is 20 bits.
    - the page offset (bottom n bits)
        - On 32 bit systems, one page is 4kb, which we can be represented with 12 bits.

- Page tables translate the logic page number to it's physical page number (also known as frame number).


## TLBs

- reading from the page table each access takes time.
    - To counteract this, processors implement a cache known as a TLB.
- TLBs is a Table Lookup Buffer

- With TLBs, we break the addresses into three parts:
    - tag (virtual page number)
    - physical page number (physical page number)
    - protection bits (protection bits, read write exec permissions)

- The TLB is usually fully associative