# 9. An Introduction to Kernel Synchronization

## What are Critical Regions and Race Conditions?

- A critical region is a code path that acesses shared data.
    - It's unsafe for multiple threads to access the same resource simultaneously.
- When multiple threads access the same shared resource simultaneously, this is known as a **race condition**.

## Locks

- A lock works similarly to a door.
    - You enter a door, lock it, then unlock it when you're done
        - While it's locked, no one else can enter. It needs to wait until it can acquire the lock.

## What is concurrency? What are the causes of concurrency?

- Concurrency occurs when multiple processes or threads execute in overlapping time periods, potentially accessing shared resources simultaneously.

### Causes of concurrency

- Interrupts: Asynchronous events can interrupt current execution.
- Softirqs and Tasklets: These are scheduled by the kernel and can run at any time.
- Kernel Preemption: The preemptive kernel allows one task to interrupt another.
- Sleeping and User Space Sync: A task may sleep, triggering the scheduler to switch contexts.
- Symmetric Multiprocessing (SMP): Multiple processors can run kernel code concurrently.

## Interrupt-safe, SMP-safe, preempt-safe

- Interrupt-safe: Code that is safe from concurrent access from an interrupt handler.
- SMP-safo: code that is safe from concurrency on symmetrical multiprocessing machines
- preempt-safe: Code that is safe from concurrency with kernel preemption is preempt-safe.

## Deadlocks, Preventing Deadlocks

- A deadlock is a condition where, for example, two threads are waiting for locks between each other.
    - Because of them waiting for locks, they cannot progress.

### Preventing Deadlocks

- Here are some basic rules to follow to prevent deadlocks:
    - implement lock ordering: Nested locks must always be obtained in the same order.
    - Prevent Starvation: Ask yourself, does this code always finish.
    - Do not double acquire the same lock.
    - design for simplicity
    - it's good practice to unlock locks in the reverse order in which they were locked.

## Contention and Scalability

- lock contention: describes a lock that's currently in use, but another thread is trying to acquire.
- If multiple threads want to access the critical area, it's going to prove as a bottle neck, or high contention.

- scalability is a measurement of how well a system can be expanded.
    - How the performance fairs as you have more processes, threads, etc.