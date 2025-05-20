# 1. Introduction to the Linux Kernel

## What is UNIX

- UNIX is an operation coined by Dennis Ritchie and Ken Thompson.
- Unix was the result of Multics, a failed multiuser operating system project by bell labs.

### Legacy of UNIX

- UNIX doesn't exist anymore, but there are new descendants:
    - UC Berkeley got access to the UNIX source course, so Berekeley released BSD in 1977.
    - Many companies, like Sun, released their own commercial version of UNIX.

## Why was unix so elegant?

- It's simpler, only having hundreds of system calls instead of thousands.
- Everything is a file, which lets us manipulate data and devices with system calls like `open()`, `read()`, `write()`, etc.
- The UNIX kernel and related system utilities are written in C, which allows for portability among different architectures.
- Process creation is fast, and uses a `fork()` system call.
- Unix provides simple and effective interprocess communication. This allows for the creation of simple programs that do one thing well.
    - For example, grep. We can take the output of a program, and pipe it into grep. 
- Unix Systems separate the policy and mechanism quite well. 

## Birth of Linux, impact

- Linux was developed by Linus Torvalds in 1991.
- It was originally an operating system for the 80386 microprocessor. 

### Impact of linux

- It has a huge impact, being found on full computers, and smaller embedded systems. 
- Linux isn't a direct descendent of UNIX.


## What is an operating System

- The operating system is the part of the system responsible for basic use and administration. This includes:
    - Kernel and device drivers
    - bootloader
    - command shell
    - basic file and system utilities.

## What is the Kernel?

- The kernel is the software that provides basic services for the other parts of the system, managing hardware and distributing system resources.
- The Kernel is the Supervisor of the OS.
- The Kernel is elevated compared to other applications. This space is known as kernel-space.
    - Kernel-space provides direct hardware access.
## Components of the Kernel?

- The kernel will typically have:
    - interrupt handlers: to service interrupt requests
    - A scheduler to share processor time among multiple processes
    - a memory management system to manage process address space.
    - other services like networking and interprocess communication.


## System Calls

- Applications will typically communicate with the kernel with system calls.
    - When an application makes a system call, the kernel executes it on behalf of the application.

## Interrupts

- The hardware can communicate with the kernel with interrupts.
    - Interrupts will interrupt the CPU.
    - The kernel will then execute a specific interrupt handler to process and respond tot he interrupt. One example is pressing a key. 
    - The kernel has the right to disable particular interrupts for synchronization purposes.
    - In linux, the interrupt handlers aren't run in kernel space, in the process context, rather having a unique interrupt context not associated with any process.

## In fact, in Linux, we can generalize that each processor is doing exactly one of three things at any given moment:

- In user space, executing user code in a process
- In kernel space, in process context, executing on behalf of a specific process
- In kernel space, in interrupt context, not associated with a process, handling an interrupt. 