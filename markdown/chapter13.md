# 13. Generic RTOS APIs

## What is a Generic RTOS API?

- An RTOS API Defines the programming interface the user interacts with when using RTOS

![alt text](image-5.png)

- The APIs from before (`xTaskCreate`, `vtaskstartscheduler()`, etc.) are the native RTOS APIs.

- The Generic RTOS Interface is a wrapper over the native RTOS API

- There are two generic APIs used:
    - `CMSIS-RTOS`: from ARM, stands for the **Cortex Microcontroller Software Interface-RTOS**.
    - `POSIX`: **Portable Operating System Interface**, which is only found is generic operating systems like linux

## Benefits of Generic RTOS APIs

- Using a generic API like CMSIS-RTOS or posix will allow for better compatability:
    - If one Application uses CMSIS-RTOS, other Applications can ues the native RTOS APIs.

## Desadvantages of Generic RTOS APIs

- RTOS vendors have no obligation to provide support for CMSIS-RTOS.
- CMSIS-RTOS updates can log behind the RTOS release cycles, which could lead to compatability issues.


