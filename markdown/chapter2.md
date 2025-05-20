# 2. Getting Started with the Kernel

## linux kernel - make utilities

```bash
make config #goes through each option, you can select yes or no.
make menuconfig #opens menu where you can choose features to add.
make gconfig #similar to menuconfig, but it's a gui.
make defconfig #makes config based on current config.
make oldconnfig #will update the existing configuration file.
```

## Kernel space - libc

- In kernel space, you don't have access to standard C functions, like `printf()`. 
- In kernel space, there's `printk()` that works similarly.

```c
printk(KERN_ERR "this is an error!\n");
```

- KERN_ERR difenise a priority flag.


## C Inline functions

- Inline functions remove the overhead of function calls (pushing PC on stack, etc.)
- We make inline functions with the inline keyword

```c
// utils.h
static inline int max(int a, int b) {
    return (a > b) ? a : b;
}
```

- Inline functions, unlike macros, are preferred for type safety and readability.

## Inline Assembly

- The C compiler embeds assembly instructions in a normal C function.
- the `asm` directive is used to write inline assembly code.

```c
unsigned int low, high;
asm volatile("rdtsc" : "=a" (low), "=d" (high));
/* low and high now contain the lower and upper 32-bits of the 64-bit tsc */
```

- Inline assembly is used for low level architecture stuff, and allow for fast path code. 

## Branch Annotation

- The C compiler has built in directives that optimizes conditional branches based on it's liklihood of being taken.
- The kernel provides two macros: `likely()` and `unlikely()`

```c
/* we predict 'error' is nearly always zero ... */
if (unlikely(error)) {
/* ... */
}
/* we predict 'success' is nearly always nonzero ... */
if (likely(success)) {
/* ... */
}
```

- You should only use these if you know the liklihood of each if statement being executed.


## Memory Protection

- In user space, it's easy to implement memory protection (SIGSEGV)
- In kernel space, the stakes are higher.
    - In kernel space, you shouldn't make illegal memory accesses, like dereferencing a NULL pointer. 
- Kernel memory is not pageable, so every byte of memory you use is one less byte of available physical memory.

## Floating Point in kernel space

- When a user space process uses floating-point instruction, the kernel handles the transition from integer to floating point mode.
    - The way it handles floating point varies by architecture, but it normally catches a trap and initiates the transition from integer to floating point mode.
- The kernel doesn't have this luxury, since it cannot easily trap itself. 
    - Therefore, you need to manually save and restore the floating point registers on top of other things.

- therefore, **unless necessary, don't use floating point operations in the kernel**.

## Stack size in kernel space.

- In user space, its easy to resize the stack.
- The kernel stack is not large nor dynamic, rather, it's small and fixed in size.

## Preemptive vs Non-Preemptive 

- Preemptive, the scheduler will interrupt the process when switching
- Non-Preemptive, the scheduler will wait for the process to complete or perform I/O before switching.

## Synchronization and Concurrency

- The Kernel allows for concurrent access of shared resources, requiring synchronization to prevent race conditions.
- The linux kernel is preemptive, so the scheduler can easily choose to take control, which could result in two threads accessing the same resource.
- Interrupt handlers can occur in the midst of accessing a resource, and the handler can get the same resource as a result.

- Solutions include using spinlocks and semaphores.

## Importance of portability - linux

- Linux is a portable operating system.
    - Therefore, the architecture independent C code must correctly compile and run on a wide range of systems. 
- There are multiple rules, like maintaining endian neutral.