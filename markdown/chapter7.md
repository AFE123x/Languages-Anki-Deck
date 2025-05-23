# 7. Interrupts and Interrupt Handlers

## Kernel Communicating with Hardware

- The responsibility of the operating system kernel is to manage the hardware connected to the machine.
- The reality is that the processor is exponentially faster than these devices. 
    - It's therefore impractical to to wait for the hardware to respond. Instead, It should do the work that it can while waiting for the devices.

- There are two approaches to working with hardware: polling and interrupts.

## Polling vs. Interrupts

- Polling: The kernel will periodically check the status of the hardware and respond accordingly
- Interrupt: the hardware will interrupt the process whenever it's ready to work with.


## Interrupts

- Interrupts enable the hardware to signal the processor.
    - For example, when you type, the keyboard controller sends an interrupt to the processor, which will have the OS respond to the new data.
- At the hardware level, there's a pin on the CPU, where the devices can send data to it.

## Interrupt Request Line

- Multiple devices can send interrupts, we need to differentiate them somehow.
- We assign each device a interupt value, known as a **interrupt request line**.
- Some devices can be dynamically assigned, like on the PIC bus.

## Exceptions

- Exceptions are similar to interrupts, but they occur synchronously with respect with the processor clock.
    - Exceptions are nicknamed **synchronous interrupts**.
- Examples of exceptions include division by zero, or page faults.


## Interrupt Handlers

Interrupt handlers are the functions the kernel runs in response to a specific interrupt.
- Interrupt Handlers are more formally known as Interrupt Service Routines (ISR).
- The interrupt handler for a device is a part of the device driver.

## Handling multiple interrupts

- Let's say we're handling an interrupt, but get another interrupt request.
- This isn't ideal, so we split interrupt handling into two parts:
    - Top half: execute immediately when interrupt occurs
    - Bottom half: deffer work to later.

### Example - network card

- When packets arrive, the card invokes an interrupt to alert the kernel.
- The top half will acknowledge and copy packets from the card's buffer into memory.
    - We can't really do any processing, since it'll take time
- The bottom half handles processing the received packets. This is done to avoid slowing down the system.


## Registering an Interrupt Handler

- Each Interrupt handler is responsible of the driver managing the hardware.

- To register a interrupt handler, and enable a given interrupt line, you use the `request_irq()` function, defined in `<linux/interrupt.h>`

```c
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char* name, void* dev);
```
- irq defines how many interrupt numbers to allocate.
- handler is a function pointer.
- flag: certain options, defined in `<linux/interrupt.h>`, which can let you, for example, disable other interrupts if you use `IRQF_DISABLED`.
- name is the name of the device (the file.)
- dev is used for shared interrupt lines.

- `request_irq` returns 0, a nonzero value indicates an error.

- **important**: It's important to create the handler in a safe context.
    - If you try calling this function in a interrupt handler, that'd be bad.
        - The implementation uses malloc, aka, memory leaks!!


## freeing IRQ

- To free an irq, you'd use `free_irq`

```c
void free_irq(unsigned int irq, void* dev);
```
- irq is the number, void* dev is for the shared interrupts.

## Implementing Interrupt Handler

```
static irqreturn_t intr_handler(int irq, void* dev);
```

- the first argument is the interrupt line number.
    - You don't really use this, but it's helping for printing log messages.
    - the dev parameter, helps differentiate between multiple devices using the same driver, hence, using the same interrupt handler.
- the return value, is of type `irqreturn_t`
    - handlers can return one of two values: `IRQ_NONE` or `IRQ_HANDLED`.
    - `IRQ_NONE` is used if the interrupt handler detects a interrupt which it's device isn't the originator.
    - `IRQ_HANDLED` returns if the device did caused the interrupt.

- For the return value, you can use the `IRQ_RETVAL(val)` which retursn the `IRQ_HANDLED` if val is nonzero, or `IRQ_NONE` if the val is zero.


## Role of interrupt handler

- The interrupt handler, at a minimum, should provide an acknowledge to the device that they received the interrupt.
- More complex devices require the interrupt to send and receive data and perform extended work in the handler.

## Shared Handlers

- There are a limited number of interrupt line numbers, so we can't assign them to every single device.
    - Instead, we can share handlers for similar devices.

- To make a shared interrupt:
    - `IRQF_SHARED` flag needs to be set when calling request_irq().
    - in the dev argument, you pass in a device structure, pointer to each device.
    - the handler should be able to distinguish whether it's device is generating the interrupt.

### Registering shared handler

- When you call `request_irq()` with the IRQF_SHARED flag specified, it will only succeed if:
    - The interrupt line isn't registered.
    - The registered interrupts also specified `IRQF_SHARED`
    - You can enable the `IRQF_DISABLE` flag to ensure mutual exclusion with the interrupt handlers.


### Distinguishing interrupts

- It's important to distinguish the interrupts.
- When the kernel receives an interrupt, it invokes sequentially each registered handler on the line.
    - Therefore, it's important to distinguish whether it generated a given interrupt. If it didn't, exit.
    - The hardware usually has a mechanism, like a status register, that can check if it invoked an interrupt.

## Interrupt Context, stack

- When you execute an interrupt handler, the kernel is put into interrupt context
    - **process context** is the mode of operation the kernel is in when executing on behalf of a process.

- In the process context, the current macro points tot he current process context.
    - It doesn't work for the interrupt context, because interrupt context is not associated with a process.
    - You also cannot sleep, since there's no backing process.
        - You can't, therefore, call functions that sleep
    
- Interrupt context is time critical, as interrupt handlers interrupt other processes.
    - Therefore, interrupt handlers should be quick and simple.
    - In the top half, you should mitigate the long stuff to the bottom half, which will run at a more convenient time.

### Interrupt stack

- In order to reduce the kernel stack size, each processor will have a stack for it's interrupt handlers.
- Similar to the kernel stack, the interrupt stack isn't big, so you should be parsimonious with memory usage.

## Procfs

- Procfs is a virtual filesystem that exists in kernel memory, and mounted at `/proc`. 

- There's a file called `/proc/interrupts` which has statistics relating to the interrupts of a system.

```
      CPU0
0:  3602371 XT-PIC  timer
```

- Here, the first column is the interrupt line used.
- The second is the number of interrupts received.
    - If there are multiple cpu cores, there'll be multiple columns.
- the third column is the interrupt handler used.
- The last column belongs to the device associated with this interrupt.

## Controlling Interrupts

- The linux kernel offers multiple interfaces for manipulating the state of interrupts in a machine.
    - These interfaces are a part of `<asm/system.h>` and `<asm/irq.h>`.

- There are many functions, like `local_irq_disable()`, `local_irq_enable()` and much more.

- We primarily use these functions to provide synchronization, and lets you guarantee that interrupt handlers will not preempt your current code.
- In reality, you shouldn't use these interfaces for newer devices. It's really only meant for older devices.

## Disable and enabling interrupts, dangers

- To disable interrupts locally for the current processor, you can use `local_irq_disable() and `local_irq_enable()`.

```c
local_irq_disable();
/* interrupts are disabled here */
local_irq_enable();
```

### dangers

- the `local_irq_disable()` is dangerous if the interrupt was already disabled prior to it's invocation.
- when you call `local_irq_enabl()`, it enables the interrupts, without considering previous states.
- Why is this bad? I don't know. It doesn't consider the previous state, i guess?

- Instead, it's better to use `local_irq_save(flags)` and `local_irq_restore(flags)`.


## IRQ_SAVE and IRQ_restore

```c
unsigned long flags;

local_irq_save(flags); /* interrupts are disabled. */
/* interrupt free goodness */
local_irq_restore(flags); /* interrupts are restored to their previous state */
```

- these are macros, so flags will be changed.
- flags is meant to contain the state of the interrupt system.
    - Some architectures incorporate stack information into the flags value, like SPARC.
- flags must be in the same stack frame, so you need to call the save and restore function in the same function.

## Disabling specific interrupt lines

- We can disable particular interrupt lines using the following functions:

```c
void disable_irq(unsigned int irq);
void disable_irq_nosync(unsigned int irq);
void enable_irq(unsigned int irq);
void synchronize_irq(unsigned int irq);
```

- `disable_irq` disables a particular interrupt line, but it waits for current handlers using the line to complete.
    - `disable_irq_nosync` does the same, but doesn't wait for the handler to complete.
- `synchronize_irq()` waits for a specific interrupt handler to exit..
- the last call to enable_irq will reenable the interrupt line.
    - Example: if disable_irq() is called twice, you need to call enable_irq twice to reenable the irq.


## Status of interrupt systems

- You can cehck the status of an interrupt using the macros:
    - `irqs_disabled()`
    - `in_interrupt()`
    - `in_irq()`

- `irqs_disabled()` checks if the interrupt system on the local processor is disabled. It returns nonzero if it is, returns zero if it isn't.
- `in_interrupt()` returns nonzero if the kernel is performance any type of interrupt handling.
- `in_irq()` returns non zero only if the kernel is specifically executing an interrupt handle≈r.3