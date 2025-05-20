# 4. Process Scheduling

## What is a process scheduler?

- A process scheduler is a kernel subsystem that puts processes to work.
- The process scheduler decides which processes to run, and for how long.

- The scheduler is responsible for dividing finite resource of processor time between the runnable processes on the system.

- Deciding which process to run next is a decision the scheduler needs to make.

## Multitasking

- multitasking is the idea that you can run multiple processes simultaneously.
- On single core processors, this gives the illusion of multiple processes running simultaneously by switching quickly.
- On multi core processors, processes can run on different cores.

### When to switch

- Processes can unrunnable, as it may be handling I/O.
- The scheduler needs to select a process that's ready to run

## Types of Multitasking

- There are two flavors of multitasking:
    - cooperative multitasking: The process doesn't stop running until it voluntarily decides to do so.
    - preemptive multitasking: The scheduler schedules when to cease and terminate a process.

### Preemptive multitasking
- Most unix systems use preemptive multitasking.
- Most systems, which preemptive schedulers, will let processes run for a certain ammount of time, known as a time slice. Once that timeslice is up, the scheduler decides which process runs next.

### cooperative multitasking

- Here, the process is expected to voluntarily suspend itself, known as yielding.
- This assumes that applications are trustworthy enough to yield the CPU, instead of keeping it to itself.
    - This could bring down systems.

## Aspects of a scheduler

- There are multiple aspects to a scheduler.:
    - speed.
    - fairness.

- Schedulers have two goals:
    - fast response time (low latency)
    - Maximal system utilization.


## Policy

- The policy is the behavior of the scheduler that determines what runs.

## I/O Bound vs. Processor-Bound processes

- We can classify processes in two classes:
    - I/O bound: spends much of its time submitting and waiting for I/O requests (Fortnite).
    - processor bound: spends much of their time executing code (little I/O).

- Usually , schedulers will run processor bound processes less frequently, but for longer durations.
    - This is because response time doesn't really matter.

- UNIX usually pfavors I/O processes, which provides good response times.
    - The same applies to Linux.

## Scheduling - Process Priority

- One approach is to assign priorities to processes, and run higher priority processes first before lower priority.
    - The kernel implements two priority ranges:
        - nice value: -20 to +19, larger nice value means lower prioritye
        - real time priority: ranges from 0 to 99, higher the number, the higher the priority.
    
## Timeslice

- Timeslice tells us how long a task can run until it's preempted.

- How much time should we allocate towards processes?
    - If the timeslices are too long, it results in poor performance.
    - Too short of a timeslice will cause a significant ammount of processor time to be wasted on the overhead of context switching.

- Another thing to keep in mind: I/O bound processes don't need long timeslices.


## Linux CFS

- CFS (Completely Fair Scheduler) is the CPU Scheduler Linux uses.

- With CFS, it decides what process to run based on what proportion of the processor the runnable processor consumed
    - If it consumed a smaller proportion of the processor, than it'll run immediately. Otherwise, it'll run later.

