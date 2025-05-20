# Chapter 5. System Calls

## What are System Calls

- System calls provide a layer between the hardware and user space processes.
- The Syscall layer provides 3 purposes:
    - Provides abstracted hardware interface for user-space.
    - Security and stability: Kernel can manage access to resources
    - A common layer between userspace and the rest of the system allow for the virtualized system provided to process.

## APIs, POSIX, C library

- Applications use a **Application Programming Interface**, or API.
    - These are implemented in user space, not directly to system calls.
- Applications can use APIs.
    - These APIs can then be implemented as a system call, or multiple system calls.
- There can be identical APIs between systems, but their implementations can differ
- System calls "provide mechanism, not policy".
### Example

- printf: printf() in the c standard library, uses the write posix function, which uses the write system call.

## POSIX

- POSIX is a series of standards from IEEE that provides a portable operating system standard roughly based on linux.


## System Calls - conventions, syscall numbers

- System calls can have zero, one or more arguments that can result in 1+ side effects.
- System calls will return a long type that signifies success or errors.
    - Usually, negative numbers indicate errors, zero indicates success.
- The C library, when a system call returns an error, will write a special error code into a global variable, `errno`.
    - We can translate this code into a human-readable error with perror().

### System call numbers

- Each system call is assigned a **syscall number**
    - It uniquely identifies the system call.
- When it comes to implementing syscalls, you cannot easily change it, as it may break applications.
    - If linux deprecates a system call, it'll simply provide a "not implemented" system call.
- A list of all the syscalls will be in `kernel/syscall_64.c`, approximately.

## System Call Handlers

- A user space application cannot directly interract with kernel code, as that would cause a security vunerability.
- Instead, the better approach would be to have the user space application signal to the kernel it wants to execute a system call.
    - This is done with a software interrupt, which will put the system into kernel mode, and run the exception handler.

```asm
movq $0, %eax
int $0x80 # invokes a software interrupt
```

- In this case, it'll look at the exception vector at index 128, which is the systemcall handler.

## System call - parameters

- When you call a system call, you typically pass parameters into them.
    - This requires us to define a convention (we'll use a certain set of registers for this)

## Communicating between user space and kernel space

- For writing into user space, you can use `copy_to_user()`
    - This take three arguments: destination memory address, source pointer in kernel space, size of data in bytes to copy.
- For reading from user space, you can use `copy_from_user()`
    - arguments are similar to `copy_to_user()`.