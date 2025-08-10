# 14. FreeRTOS Memory Management

## What is static memory

- Static memory is memory who's lifespan is the entire duration of a program.
    - This includes global variables, and variables declared inside functions with the static keyward. .

```c
uint8_t globalVar = 12;

void myfunc(void){
    static uint8_t staticVar = 0;
}
```

- the issue with static memory is it wastes space (since it'll live throughout the lifetime of the program, even if it isn't necessary)

## Stack Memory

- Stack memory is memory where it's lifespan is within the function.
- this includes local variables

```c
void myfunc(void){
    uint8_t localvar = 0; //stored on the stack.
}
```

## Heap memory

- heap memory is used when dynamically allocated memory is needed.
- For example, `xTaskcreate()`allocates the Thread Control Block (TCB) in the heap.

- On the MCU RTOS system, there are usually two heaps:
    - FreeRTOS heap: Used for creating tasks and other FreeRTOS primitives
    - System heap: defined in startup and linker script. they won't be available for the final application code. 


## FreeRTOS - Heap Implementations

- Under FreeRTOS, there are multiple variations of the heap. 
- Some will let you free, others won't let you free memory.
- Some will coalesce free space, some won't

- It's your decision what you want. 


## Monitoring Stack space.

- You can use `vApplicationStackOverflowHook()` to halt the application.
- you enable this by setting `configCHECK_FOR_STACK_OVERFLOW` to 1 or 2:
    - `#define configCHECK_FOR_STACK_OVERFLOW 1`
    - `#define configCHECK_FOR_STACK_OVERFLOW 2`

- `#define configCHECK_FOR_STACK_OVERFLOW 1` will simply check the stack pointer at the end of the task.
- `#define configCHECK_FOR_STACK_OVERFLOW 2` will check the stack pointer and add a watermark at the beginning of the stack. if it was changed, it means stack overflow happened.

- You would then implement the `vApplicationStackOverflowHook()` to handle this. 

## Monitoring Heap Space

- you can check for heap related issues using the `vApplicationMallocFailedHook()` to detect malloc failures

- You'd enable the setting by:
    - `#define configuUSE_MALLOC_FAILED_HOOK 1` setting macro first
    - `vApplicationMallocFailedHook()` define the handler function.

- There are two functions for monitaring the heap:
    - `xPortGetFreeHeapSize()` monitors available heap space
    - `xPortGetMinimumFreeHeapSize()` monitors least amount of free heap space every available.

## Memory Protection Unit (MPU)

- You can monitor memory accesses using a piece of hardware known as the Memory Protection Unit.
    - If the hardware detects an invalid access, it'll interrupt the system.

- You introducet he restrictions suing the `xTaskCreateRestricted`, which will create a restricted task.