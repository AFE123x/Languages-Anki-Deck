# 13. Managing Memory

## swapping - introduction

- swapping involves reserving some storage so the kernel can place pages of memory not mapped to a file, freeing up memory for other usage.
- It's not ideal, as using too much memory causes issues, like **disk thrashing**.
    - this is where you write frequently to swap, as a result of not enough ram.

## swapping - zram

- the zram driver will create RAM based block devices, named `/dev/zram0`, where pages are compressed prior to being stored.

- you can then mount them via the fstab.

## mmap

- `mmap` lets a process manipulate it's memory map

```c
void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
```

- this function will map `length` bytes of memory from the file with a descriptor, `fd`.
    - length is rounded up to it's nearest whole number of pages.
- `prot` is the read, write and execute permissions.
    - it can also include `MAP_SHARED` or `MAP_PRIVATE`.


- If you want to allocate private memory, you can set fd to -1, which will ignore the file descriptor.

## mmap - sharing memory

- you can add the `MAP_SHARED` flag to share memory between processes.

```c
int shm_fd;
char *shm_p;
int main(int argc, char** argv){
    shm_fd = shm_open("/myshm", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, 65536);
    shm_p = mmap(NULL, 65536, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
}
```

- in another process, you would then use shm_open() to open the file descriptor again.


## Accessing Device memory with mmap

- you can access the memory space using mmap.


```c
int f;
int fb_size;
unsigned char *fb_mem;
int main(int argc, char** argv){
    f = open("/dev/fb0", O_RDWR);
    /* Use ioctl FBIOGET_VSCREENINFO to find the display dimensions and calculate fb_size */
    fb_mem = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    /* read and write pixels through pointer fb_mem */
}
```

## checking for memory leaks

- there are multiple ways to check for leaks
    - `mtrace`, traces calls to malloc and free.
    - `valgrind`: contains memcheck and other things in general, like cache hit rates, multithreading related issues.