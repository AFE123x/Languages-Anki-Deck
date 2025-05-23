# 8. Bottom Halves and Deffering Work

## What are the limitations of interrupt handlers?

- Interrupt handlers run asynchronously, which will interrupt other potentially important code
    - This includes other interrupt handlers
- The interrupt handlers run wit hthe current interrupt level disabled
    - If the handler's shared, that's pretty bad since it prevents other hardware to communicate with the OS.
- Interrupt handlers are time critical since they deal with hardware.
- Interrupt handlers don't run in the process context, so they can't block.

- Due to these limitations, we need to divide the our approach to managing interrupts into two halves.
    - The top half, which is called by the kernel asynchronously in response to a hardware interrupt.
    - The bottom half, which performs any interrupt related work not performed by the interrupt handler.


## How to Divide work between top and bottom half?

- Ideally, we want the top half to do as little work as possible.
    - In reality, the device expects some acknowledge or copy data from/to the hardware.
- Therefore, we need to do some work in the interrupt handler.

- Everything else besides copying or acknowledge data, can be done in the bottom half.
    - For example, you can do data processing in the bottom half.


### Rule of thumb

- if the work is time senstive, or related to the hardware, do the work in the interrupt handler.
- If the work needs to ensure another interrupt does not interrupt it, do it in the handler.
- Everything else, you can do it in the bottom half.


## Implementing a Bottom half

- There are two approaches to implementing a bottom half:
    - softirqs: They're statically defined bottom halves that can run simultaneously on any processor.
        - Two of the same kind of softirqs can run concurretly.
        - It's good for performance, but they can run concurrently and softirqs need to be compiled with the kernel.
    - tasklets: flexible, dynamically created bottom halves built on top of softirqs.
        - tasklets cannot run simultaneously.
        - You don't need to compile it with the kernel.

## softirqs

- Softirqs aren't very common to use, tasklets being much more common.

- Softirqs are represented using the **softirq_action** structure:

```c
struct softirq_action{
    void (*action) (struct softirq_action* );
};

static struct softirq_action softirq_vec[NR_SOFTIRQS]; //you can register up to 32 softirqs
```

- NR_SOFTIRQS is a macro, typically 32.
    - The limit is 32, but nine exist at the moment.
- action is a function pointer to a function of type `void softirq_handler(struct softirq_action*)`.

- Why pass in the softirq_action structure? so that you can enable future additions to the structure with requiring a change in every softirq handler.

## How are softirqs executed?

- Before executing the softirq, you need to mark it, which is known as **raising the softirq**.
    - Usually, an interrupt handler ill mark the softirq for execution before returning, which will then let the softirq run at an appropriate time.

- The softirqs are checked and executed in the following places:
    - Returning from a hardware interrupt.
    - In the `ksoftirqd` kernel thread.
    - Manually by subsystems that explicitly check for them.

## When to use softirqs

- Softirqs are reserved for the most timing-critical and important bottom holf processing on the system.
    - There are really only two:
        - networking
        - block devices.

- If you want to add a new softirq, you nede to ask yourself why using a tasklet is insufficient.
- tasklets are dynamic and easier to use for their weaker locking requirements, and they can perform well.

## Registering softirq handlers

- To add a softirq, you use the `open_softirq()` function

```c
open_softirq(NET_TX_SOFTIRQ, net_tx_action);
open_softirq(NET_RX_SOFTIRQ, net_rx_action);
```
- The first argument is the softirq index, which is an enum.
- The second argument is the handler function.


## running softirq

- To run a handler, after registering it with `open_softirq()`, you can run it with `raise_softirq()`

```c
raise_softirq(NET_TX_SOFTIRQ);
```

- This tells the kernel to run the handler associated with NET_TX_SOFTIRQ at the next time the kernel executes softirqs.
- this function will disable interrupts priori to raising the irq, and restore the interrupts to their previous states.
    - If you don't want to do this, and you know the interrupts are already off, you can use `raise_softirq_irqoff(NET_TX_SOFTIRQ)` instead.

## Tasklets

- Tasklets are a bottom-half mechanism that builds on top of softirqs.
    - Unlike softirqs, tasklets are simpler and the locking rules are relaxed.
- For most cases, tasklets are going to work fine.

## Implementing tasklets

- Tasklets are represented with two softirqs: `HI_SOFTIRQ` and `TASKLET_SOFTIRQ`.
    - `HI_SOFTIRQ` based tasklets are run before `TASKLET_SOFTIRQ` based tasklets.

### Representing tasklets

- We use the `tasklet_struct` for this

```c
struct tasklet_struct{
    struct tasklet_struct* next;
    unsigned long state;
    atomic_t count
    void (*func)(unsigned long);
    unsigned long data;
}
```
- the func is the tasklet handler.
- data is the handlers sole argument.
- state is either `TASKLET_STATE_SCHED` or `TASKLET_STATE_RUN`
    - `TASKLET_STATE_SCHED` is a tasklet that is scheduled to run.
    - `TASKLET_STATE_RUN` is a tasklet that is currently running.
        - This is really only used on multiprocessor machines.
- The count field is a reference count for the tasklet.
    - If it's nonzero, the tasklet is disabled and cannot run.
    - If it's zero, the tasklet is enabled and can run if marked as pending.

### Creating tasklets

- You can create tasklets using the following macros:

```c
DECLARE_TASKLET(name, func, data); //gives count of zero, enabling tasklet.
DECLARE_TASKLET_DISABLED(name, func, data); //gives count one, disabling the tasklet
```
- name is the name of the tasklet variable, func is the handler, data is just an unsigned long value.

- If you dynamically allocate the tasklet, you'd use `tasklet_init` to initialize the tasklet.
```c
tasklet_init(t, tasklet_handler, dev); 
```

### Storing Tasklets

- Tasklets are two in one of two structures (lists):
    - `tasklet_vec` for regular tasklets
    - `tasklet_hi_vec` for high priority tasklets.

### Scheduling tasklets, disabling

- We schedule tasklets via the `tasklet_schedule()` and `tasklet_hi_schedule()`.
    - It takes tasklet_struct as their lone argument.

```c
tasklet_schedule(&my_tasklet); //will run the tasklet in the future.
tasklet_disable(&my_tasklet); //will disable the tasklet.
tasklet_kill(&my_tasklet); //will kill the tasklet.
```

## Enabling and disabling bottom halves

- to enable and disable bottom halves, you have two functions:
    - `local_bh_disable()`: disables softirqs and tasklets on the local processor
    - local_bh_enable(): Enalbes softirqs and tasklets on the lolal processor