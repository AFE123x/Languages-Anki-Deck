# 2. Introducing Super Loop Programming

## What is the super loop program? Consequences of superloops

- This is your classic embedded program, where you have a `while(1)` loop that never stops.
- It's simplistic, but isn't great for a real time system.
    - When you call a function in an order, it'll repeat in the same order. This is nice, but if one function deals with a delay, it'll affect the subsequent functions.

- The super loop is especially problematic with polling. 


## Interrupts

- Polling is a waste of CPU cycles, and results in a less responsive system.
- Interrupts let you stop the main flow of the program, and execute an Interrupt Service Routine.
- Interrupts need to be short to minimize the time of the interrupt
    - this prevents the interrupt interrupting the interrupt.
- You can also assign the interrupt handlers with a priority level so high priority interrupts run first.


## Implementing interrupts with super loops

- What you can do is use ISRs to set flags, which can then be checked by another part of code in the super loop.

## DMA

- DMA stands for **Direct Memory Access**

- Instead of the CPU managing a series of CPU reads/writes, the CPU can ask the DMA controller to transfer a number of bytes from a device to memory independently.


## RTOS advantage over super loops

- RTOS has two main differences compared to super loops:
    - Each task will have its own stack
    - you can assign priorities on tasks.

## Round Robin Scheduling

- Round Robin is an approach of scheduling where you run a task for a fixed time slice, then switch to another task. The scheduler chooses what's next.


## Preemptive based scheduling

- A preemptive based scheduler is a scheduler that can interrupt the current running process on the CPU.
- 