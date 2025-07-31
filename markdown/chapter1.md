# 1. Introducing Real Time Systems

## What is real-time?

- A system is considered "real-time" if the system has a deterministic response to a given event.
- If the system is considered to fail if it doesn't meet a time requirement, it's real-time.

### Examples

- One example is controlling the temperature of a soldering iron.

- The MCU will be responsible for the following:
    - taking a reading from the temperature sensor via an ADC
    - Running a closed loop control algorithm
    - adjusting the output of the heater.


- For control systems, we want to have a deadline for when to do the tasks. This is real-time.


## When to use RTOS

- First off, using a microcontroller might not be necessary.
    - For example, you could use a motor and sensor circuit for a car window.
- If you need to use a microcontroller, first figure out if you could just use a simple for loop to solve the problem.
- **Before using RTOS**, see if you could solve the problem without it.

## Types of real time systems

| type | meaning |
| hardware | using digital logic, ASIC, FPGA, etc. for real time system |
| bare-metal | using firmware that isn't built on top of a preexisting kernel/scheduler |
| RTOS-based | Firmware that runs a scheduling kernel on the MCU |
| RTOS-based software | Software running on a full OS with an MMU and CPU. |


## what differentiates a Real Time Operating System from a standard Operating System

- Similar to RTOS, a standard OS will have the libraries and features one could want.
- However, the standard OS misses out on the strict focus on meeting timing requirements

- Another example is with Mutex locks.
    - On a standard OS, a thread will wait for the Mutex to become available indefinitely.
    - On an RTOS system, they only allow for a maximum delay to be specified.

- For example, thread 2 holds the lock. Let's say thread one is set to wait for the Mutex for only 100 ms.
    - if the Mutex lock isn't acquired within the window, thread 1 times out.

## Variants of RTOS systems

- Hard real time systems
- Soft real time systems
- Firm real time systems

## Hard real time systems

- A hard real time system is required to meet its deadline 100% of the time.
    - If it doesn't, it's considered to have failed.

- Examples of a hard real time system can be found in medical devices.
    - If the system misses a deadline, it could kill someone.

## Firm real time systems

- Here, it's not as strict, as firm real time systems need to hit their deadlines **nearly** all the time.
    - Periodically, it will fail, but it won't be detrimental.

## Soft real time systems

- Soft real-time systems have the most lax when it comes to how often the system needs to meet its deadline.
- For example, cruise control. It doesn't need to be perfect, but it should be within the range.