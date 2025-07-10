# 16. Real-Time Programming

## What is real time

- we consider a task a real-time task if it has to complete before a **deadline**
    - One example is playing an audio stream, where you pass in a buffer to the audio buffer.

- missing deadlines for a realtiem task has consequences

## real time tasks - consequences categories

- **soft realtime**: deadline is desireable, but isn't a failure if deadline is missed.
- **hard real-time**: missing a deadline is detrimental, and can have severe consequences.

## hindrances to real time programming, areas where problems could arise

- real time programming is about ensuring threads controlling the output in real time are scheduled when needed so it can complete the job before the deadline.
- hindrances are problematic.
* hindrances include:
    * **Scheduling**: Use real-time policies (`SCHED_FIFO`, `SCHED_RR`) and assign priorities based on deadline (Rate Monotonic Analysis).
    * **Scheduling latency**: Kernel must quickly reschedule on events (e.g., interrupts); avoid unbounded delays.
    * **Priority inversion**: High-priority threads blocked by low-priority ones—use priority inheritance (e.g., rt-mutexes).
    * **Accurate timers**: Essential for precise deadline tracking; use high-resolution timers.
    * **Page faults**: Can disrupt timing; prevent by locking memory.
    * **Interrupts**: Can add unpredictable overhead; mitigate via:
    * Running as kernel threads
    * Shielding CPUs from interrupts (on multi-core systems)
    * **Processor caches**: Cause timing variability, especially on multi-core; generally hardware-dependent.
    * **Memory bus contention**: DMA/peripheral access can delay CPU memory access; also a hardware-level issue.

## real time hurdle - scheduling latency

- real time threads need to be scheduled as soon as it has to do something.
- problem is, there's always delay between when the wakeup occurs to the time the thread starts to run (hardware or interrupts for example).
    - this is known as **scheduling latency**

## real time hurdle - kernel preemption latency

- kernel preemption latency occurs because it won't be safe or desirable to preempt the current thread for the scheduler.
- The kernel has three settings for preemption.
    - `CONFIG_PREEMPT_NONE`: once a thread enters kernel mode, it runs to completion until it exits or voluntarily sleeps.
        - poor response time.
    - `CONFIG_PREEMPT_VOLUNTARY`: it'll check periodically if a higher priority thread is ready to run
        - middle ground
    - `CONFIG_PREEMPT`: the kernel can be preempt immediately.
        - it's fast, but there's overhead with context switching.


## real time linux kernel

- There's a special version of the kernel for real-time needs, presented as a kernel configuration option, `PREEMPT_RT`
- this version of the kernel reduces the amount of time the kernel spends in an **atomic context**, where it isn't safe to call the scheduler.

- rt linux does the following:
    - reduced the impact of interrupt handlers by turning them into kernel threads.
    - make locks preemptible, so thread can sleep while holding lock.

- rt linux makes average-case interrupt handling overall slower, but it's much more deterministic.

## atomic context

- The Atomic context is context where it isn't safe to call the scheduler.

- Cases include:
    - running interrupt or trap handler
    - holding onto spinlock, or in RCU critical section.
    - between calls to preempt_disable() and preempt_enable()
    - hardware interrupts are disabled.


## threaded interrupt handlers

- interrupt handlers can take time from real-time tasks.
- threaded interrupt handlers provide a priority with them so it can be scheduled for an appropriate. time.

- instead of using `request_irq()`, you'd use `request_threaded_irq()`.
- you can also set it as the default option via kernel configs.

## preemptible kernel locks

- the rt linux kernel make locks preemptible
- this gets rid of spinlocks, as it takes up time.


## High-Resolution Timers

* For applications that require precise timing, **high-resolution timers** should be used.

* By default, Linux uses a system timer that ticks at a configurable frequency—typically **100Hz** on embedded systems.

* The time between these ticks is known as a **jiffy**.

* To provide greater accuracy, Linux supports **high-resolution timers**, which are enabled if a suitable hardware timer and driver are available.

  * This feature can be enabled in the **kernel configuration**.

* When high-resolution timers are active, the system can schedule events with timing accuracy that matches the **granularity of the hardware timer**, allowing for much finer control than traditional tick-based timers.


## Avoiding page faults

- a page fault occurs when an application read/writes to memory not committed to.
- It's impossible to predict when a page fault happens.

- You can lock down memory used by the process, called `mlockall()`.
    - you can pass in two flags:
        - `MCL_CURRENT`: locks all pages currently mapped
        - `MCL_FUTURE`: locks pages mapped in later.

- You can use this by growing the stack to a size larger than you think you'll ever need, then use `mlockall()`

## Interrupt Shielding

- given a multi core processor, you can dedicate one or more cores to only handle real time tasks.
    - aka, eliminating interrupts from a core

- this is done with the `taskset` utility, or the `sched_setaffinity()` and `pthread_setaffinity_np()` function.