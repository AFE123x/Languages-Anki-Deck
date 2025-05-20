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


### freeing IRQ

- To free an irq, you'd use `free_irq`

```c
void free_irq(unsigned int irq, void* dev);
```
- irq is the number, void* dev is for the shared interrupts.

