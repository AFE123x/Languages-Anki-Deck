# 3. Process Management

## What is a process?

- A process is a program in the midst of execution.

- Processes do more than executing program code (known as the text section in unix).
    - Processes also included:
        - files
        - pending signals
        - internal kernel data
        - processor state
        - memory address space
        - threads
        - data section containing global variables.

- Processes aren't the program itself, rather, it's a running program.

### threads

- The threads do the actual executing of the program
- Each thread contains:
    - unique program counter
    - Process stack.
    - Set of processor registers.

- The kernel schedules threads rather than processes.

- Linux doesn't differentiate threads and processes, rather just think of a thread as a special kind of process. 

## Virtualization

- On most modern Operating Systems, processes provide two virtualizations: Virtualized processor and virtual memory.
    - Virtual processes gives the process the illusion it monopolizes the system.
    - Virtual memory lets the process allocate and manage memory as if it alone owned all the memory. 

## Process - starting up one, end of one

### Birth
- Processes are created via with the `fork()` system call.
    - `fork()` duplicates the existing one.

- The process that calls fork() is the parent, whereas the new process is called the child.
    - The parent will continue execution, and the child will start execution at the same place. 

- To execute a program, the `exec()` function creates the new address space and loads the new program into it. 

### death
- A program exits via the `exit()` system call.
- Here, the function terminates the process and frees all its resources.
- The parent can inquire about the status of the child via the `wait()` system call. 


## Process Descriptor and Task Structure

- The kernel stores the list of processes in a circular doubly linked list called the task list.
- Each element in the task list is a process descriptor with the type `struct task_struct`, defined in `<linux/sched.h>`. 
    - The process descriptor contains all the information about a specific process. 
- The process descriptor contains information describing the executing program: 
    - Open files.
    - Address space.
    - pending signals.
    - process states.


- The elements in the task list are now allocated via the slab allocator. 

- Due to architecture related hurdles, thread_info was introduced, which holds a pointer to the task_struct. It's located on the bottom of the kernel stack. 


## Storing the Process Descriptor

- The system uniquely identifies processes with a process identification value, or PID. 

- PID is usually an int type, but the default maximum value is 32768, due to backwards compatability, and it being a short int.
    - The value can be increased to 4 million, and can be controlled in `<linux/threads.h>`.
- The PID is an opaque type

## Opaque types

- An opaque type is a data type whose physical representation is unknown or irrelevant.

## Current macro

- The task list is a linked list, hence it's slow to access things.
- Instead of iterating through the whole list, we can access the task entry via the `current` macro. 

## Process State

- the state field of the process descriptor describes the current condition of the process. It's represented with one of 5 flags.
    - `TASK_RUNNING`: The process is current running or it's able to run.
    - `TASK_INTERRUPTIBLE`: The process is sleeping, waiting for some condition to exist. Once this condition exists, the kernel sets the process state to `TASK_RUNNING`.
    - `TASK_UNINTERRUPTIBLE`: this means that the process will not wake up and become running if it receives the signal.
        - This usually happens if the process needs to wait without interruption. 
    - `TASK_TRACED` means the process is being traced by another process. For example, a debugger.
    - `TASK_STOPPED`: This means that the process not running or eligible to run.
        - For example, if the process receives the SIGSTOP, SIGTSTP, SIGTTOU, SIGTTIN signal.

## Setting Current Process state

- we can set the process state (TASK_RUNNING, TASK_STOPPED, etc.) via the `set_task_state` function.

```c
set_task_state(task, state)
set_current_state(state) //works the same, but for the current process.
```

- These are a part of `<linux/sched.h>`

## Process Family Tree

- There exists a heirarchy between processes in the unix system.
- All processes are a descendant of the init process, who's PID is one.
    - The init system will read the system initscript and execute more programs.
- All processes on the system have exactly one parent, and processes will have 0 or more children. 
- Each tast_struct has a pointer to it's parent task_struct, named parent. It also has a list of children task_struct.

```c
struct task_struct *my_parent = current->parent;

struct task_struct* task;
struct list_head* list;

list_for_each(list, &current->children){
    task = list_entry(list, struct task_struct, sibling);
}
```

## Process Creation

- UNIX takes a unique approach

### Typical Approach
- Most Operating Systems will have a spawn mechanism to create the new process, address space, read and load the executable.

### UNIX Approach

- Unix takes a unique approach, putting these steps into two distinct functions: `fork()` and `exec()`.
    - `fork()` creates a child process, which is a clone of the parent task, but only differs with it's PID.
        - The child process has it's Parent PID (PPID) to the process that called `fork()`.
    - `exec()` will load the program into memory.


## Fork: Copy-on-Write

- Traditionally, when `fork()` is called, all the resources awned by the parent are duplicated and it's copy is given to the child.
    - This is wasteful as it copies a lot of data that'd probably be shared.
        - This is especially true if `exec()` is called immediately after `fork()`.

- Copy-on-write delays the copying unless the data is written to in the child process.
    - If not, the child and parent process will share the same address space.

- TD;LR: copy-On-Write will only copy the memory if the child process needs to write on it. Otherwise, it's not copied, which results in a speedup. 

- With this technique, the only overhead incurred by `fork()` is the duplication of the parent's page table and the creation of the unique process descriptor for the child

## Forking implementation

- Linux implements `fork()` with the `clone()` system call. 
- `clone()` provides more lower level control of process creation.

- majority of `fork()` is done in `do_fork()`, which uses `copy_process()`.
- After the fuction is done, and it succeeds, the kernel will run the child process, then the parent process.

### `copy_process()`

- First, it calls `dup_task_struct`, which creates a new kernel stack, thread_info structure and task_struct for the new process.
- Second, it checks that the new child will not exceed the resource limits on the number of processes for the current user.
- Various members of the process descriptor are cleared or set to initial values.
- The child state is set to `TASK_UNINTERRUPTIBLE`, to ensure it doesn't run.
- fourth, the function copies the flag member of the task_struct.
- fifth, it allocates a PID number to the new task. 
- sixth, depending on the flags passod to clone, it'll either duplicate or share open files, address space, etc.
- Finally, `copy_process()` cleans up, and returns a pointer to the new child.

## Linux - Threads

- Threads are a popular programming abstraction, letting multiple threads execute the same program concurrently, sharing the same address space.
- Linux implements it similar to processes, with the exception of shared memory. 
- Each thread will have a `task_struct`, similar to a process, but have the shared address space.


### Creating a thread

- We can use the `clone()` system call to create a thread.

```c
clone(CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND, 0); //this is how threads are created.

clone(SIGCHILD, 0); //similar to fork()
clone(CLONE_VFORK | CLONE_VM | SIGCHILD, 0); //similar to vfork. 
```

## Kernel Threads

- It's useful for the kernel to be able to perform some operations in the background. This sis do via **kernel threads**.
    - These threads only exist in kernel space. It differs as the threads don't have an address space.
- The kernel threads only operate in kernel space, and do not context switch into user space.
- Kernel threads, however, are schedulable and preemptable, similar to normal processes. 
- Kernel threads are created on boot, and kernel threads can create other kernel threads.

- Kernel threads are handled by the `<linux/kthread.h>` library.


## kthread.h

- pthread is a user space library. kthread is the kernel space version.

### Creating thread

- kernel threads are spawned with `kthread_create`

```c
struct task_struct* kthread_create(int (*threadfn)(void* data), void* data, const char namefmt[], ...);
```

- kthread_create will create the thread, but it'll be in an unrunnable state.

- To create a runnable thread, you'd use `kthread_run`

```c
struct task_struct* kthread_run(int (*threadfn)(void* data), void* data, const char namefmt[], ...);
```
- kthread_run calls kthread_create, and a function known as `wake_up_process()`, which will actually run the thread. 

### Stopping thread

Threads are stopped in two ways:

```c
do_exit(); //called by thread
kthread_stop(struct task_struct* k) //will kill the thread, from the caller, not the thread itself. It uses the struct created when kthread_run was called.
```

## Process Termination

- Processes die, how do we handle this?

- Process termination is done ideally with the `exit()` system call.
    - The compiler places this after main returns.
- This assumes the program exits successfully. Things can happen, like signal interrupts.
    - Regardless of how the process terminates, it'll usually use the `do_exit()` function, which does the following:

### do_exit procedure

- Set the PF_EXITING flag in the flags member of the task_struct.
- Calls del_timer_sync() to remove kernel timers.
- removing accounting information (if BSD process accounting is enabled)
- release the `mm_struct` held by process with `exit_mm()`
- call `exit_sem()` if the process is queued waiting for an IPC semaphore.
- calls `exit_files`, to decrement the file descriptor usage counter.
    - If it's zero, it's destroyed.
- Set the exit code.
- call `exit_notify()` to send signals tot he task's parents, and reunite the dying threads tasks to another thread in the group. 
- call `schedule()` to switch to a new process.


### Removing the Process Descriptor

- Once `do_exit()` is complete, the process descriptor for the terminated process still exists, and it's a zombie, and the process can't run.

- When it's ready to release, the parent process call `release_task`

### release_task

- It calls detach pid to remove the process from the pidhash, removing the process from the task list. 
- exit_signal releases remaining resources used by the dead process.
- If the task was the last member of a thread group, and the leader is a zombie, the nrelease_task will notify the zombie leader's parent.
- `put_task_struct()` is called to free the page containing the process's kernel stack, and the thread_info structure, also deallocating the slab cache containing the task_struct.

## Wait

- The wait system call tells the parent process to suspend until the child process is terminated. 

