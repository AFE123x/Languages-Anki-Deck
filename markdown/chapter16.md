# 16. Advanced IRQ Management

## What is an interrupt controller

- There are many IRQs the CPU needs to manage. 
- So much that there's a piece of hardware that manages it, known as an **interrupt controller**

- The **interrupt controller** has multiple facilities, such as:
    - Masking/unmasking individual interrupts
    - Setting priorities
    - SMP affinity
    - other things, like wake up interrupts

## what are the two types of interrupts

- **Synchronous interrupts**, also called **exceptions**, are produced by the CPU when processing instructions.
    - These are **non-maskable interrupts**, and result from hardware failures.
    - The CPU processes these
- **asynchronous interrupts**, which are issued by other hardware devices. 

## Synchronous Interrupts: classifications

- **Processor-detected Exceptions**: the CPU generates these due to erroneous condition.
    - For example, faults (invalid instruction), traps (invalid memory accesses), aborts.
- **Programmed exceptions**: requested by programmer, and handled like traps.


## IRQ - struct representations

- `struct irq_chip`: implements a set of methods on how to drive interrupt controller.
- `struct irqdomain`: provides mapping between hardware and kernel interrupts.
- `struct irq_desc`: Linux view of an interrupt
- `struct irq_action` Describes IRQ handler (populated when you request an IRQ)
- `struct irq_data`: provides IRQ related data.

## Chained IRQs

- Chained interrupt is the concept of the interrupt handler of a parent calls the child interrupt handler, that that child calling its child, and so on.
- This approach is usually used for internal components, that need an immediate response without overhead.

## Nested interrupts

- This is usually used by:
    - Controllers that sit on slow buses (like I2C).
    - Devices who may need sleep.

- Nested interrupts are handlers that don't run in the HW concepts. 
    - Instead, they're run in a thread, and can be preempted.
