# Chapter 4 - The Abstraction: The Process

## What is a process?

- A process is a running program.
    - A program is a set of instructions for the CPU to execute.


## Why do we virtualize the CPU?

- We want the illustion that we're running more than one program at a time.
    - The OS does this by virtualizing the CPU.

- We do this by:
    - running one process
    - stoping it and running another
    - repeat

- This technique is known as time sharing of the CPU, which lets users run as many concurrent processes as they'd like.

##  What is a mechanism and policy?

- A policy is the high level decision making rules that guides the system's behavior
- Mechanisms are the underlying implementation used to enforce the policies.

- Example:
    - The CPU Scheduling algorithm is a policy
    - Context switching is the mechanism that lets your scheduler work.

## What is a context switch?

- A context switch is a mechanism that gives the OS the ability to stop running a program and start running another one on a given CPU.

## Machine states

- To understand what constitutes a process, we need to understand it's machine state.

- The machine state consists of the following:
    - memory: what memory can the process address (address space)
    - registers: what's in the registers while the process is running.
    - I/O information: Files, persistant storage devices, etc.


## Process APIs

- The Operating System should ideally provide the following APIs:
    - **create**: The OS should provide some function that lets you create a new process.
    - **destroy**: Some way to forcefully destroy the process.
    - **wait**: We may want to wait for the process we created to finish.
    - **miscellaneous controls**: We may want other ways to control the process.
    - **status**: Some way to get status information (how long the process has run for).


## How are programs transformmed into processes?

- First, the OS needs to load it's code into memory + static dat.
- Initially, the program will reside on the disk.
    - Older operating systems will load the process **eagerly**, and load everything
    - Newer operating systems will load it when needed, or **lazily**.
- Once the instructions are loaded in memory, we need to allocate the programs stack and heap.
- It may also need to do some tasks relating to IO:
    - On UNIX Systems, each process will have 3 file descriptors:
        - stdout
        - stdin
        - stderr.
- Once all of this is done, the program is ready to execute the program.

## Process States

- A process state describes what the process is doing at a given time.
- In a simplified view, a process can be broken down into three states:
    - running: It's currently executing instructions in the CPU
    - ready: The process isn't running, but it's ready to run.
    - blocked: The process is doing some sort of I/O, and isn't ready to run.

https://o.quizlet.com/szflbuHPkWtFEa6ageTh3g.png

## OS Data Structures - Processes

- The OS needs to keep track of the relevant information regarding processes.
- It may look something like this:

```c
// the registers xv6 will save and restore
// to stop and subsequently restart a process
struct context {
    int eip;
    int esp;
    int ebx;
    int ecx;
    int edx;
    int esi;
    int edi;
    int ebp;
};
// the different states a process can be in
enum proc_state { UNUSED, EMBRYO, SLEEPING,
RUNNABLE, RUNNING, ZOMBIE };
// the information xv6 tracks about each process
// including its register context and state
struct proc {
    char *mem; // Start of process memory
    uint sz; // Size of process memory
    char *kstack; // Bottom of kernel stack
    // for this process
    enum proc_state state; // Process state
    int pid; // Process ID
    struct proc *parent; // Parent process
    void *chan; // If !zero, sleeping on chan
    int killed; // If !zero, has been killed
    struct file *ofile[NOFILE]; // Open files
    struct inode *cwd; // Current directory
    struct context context; // Switch here to run process
    struct trapframe *tf; // Trap frame for the
    // current interrupt
};
```
- We'll have a structure for the registers
- Another structure will have the location of the kernel stack, size of process memory, start of process memory, the state, etc.