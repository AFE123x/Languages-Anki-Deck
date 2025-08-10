# 6. The FreeRTOS Scheduler

## Creating tasks and starting scheduler - general steps
 
- The general steps are as follows:

1. Initialize MCU hardware
2. Task functions need to be defined
3. RTOS tasks need to be created and mapped to functions defined in step two
4. start the scheduler

## Defining task functions

- In RTOS, we want to define the tasks, and what they should do. 

```c
void greentask(void* argument){
    while(1){
        /* task logic*/
        if(shouldfinish){
            vTaskDelete(NULL); //task deletes itself
        }
    }

}
```

- here, we have a task named `greentask` where we pass in a pointer to the arguments. 
- Most times, you want to have a `while(1)` loop so the task doesn't finish. 
- if you want the task to finish and delete, you'd call the `vTaskDelete()` function, passing in `NULL`.

## Creating Tasks

```c
BaseType_t xTaskCreate( TaskFunction_t pvTaskCode, const char * const pcName, configSTACK_DEPTH_TYPE  usStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask);

BaseType_t retVal = xTaskCreate(Task1, "task1", StackSizeWords, NULL, tskIDLE_PRIORITY + 2, tskHandlePtr);
```

- This is to create a task.
- configStacksize is the size of the stack
- you can pass in parameters, but you should ensure the parameter is valid through the task's lifetime.
- you can set the priority level.

- The return type can be used to check if the task creation was successful.

## Starting Scheduler

- Once you create all the tasks, you can start the scheduler with `vTaskStartScheduler();`
- this function should not return.

## Deleting Tasks

- You can delete tasks from the scheduler using `vTaskDelete(handler)`
- If you want a task to delete itself, you pass in `NULL`.
- otherwise, you pass in a TaskHandle_t type, which you create during the xTaskCreate funciton


```c
TaskHandle_t blueTaskHandle;
int main(void){
    HWInit();
    assert_param(xTaskCreate(BlueTask,"BlueTask", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &blueTaskHandle) == pdPASS);
    assert_param(xTaskCreateStatic(RedTask,"RedTask", STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, RedTaskStack, &RedTaskTCB) == pdPASS);
}

void RedTask(void* argument){
    vTaskDelete(blueTaskHandle); //removes bluetask from scheduler.
}
```

## heap allocated stack

- When we create a task, we pass in the stack size.

```c
xTaskCreate(Task1, "task1", StackSizeWords, TaskHandlePtr, tskIDLE_PRIORITY + 2, NULL);
```

- The stack is allocated dynamically via heap memory.
    - this means the stack is allocated during runtime

## statically allocated task

- `xTaskCreate` will allocate heap memory for the stack. Some people may not want it for some reason. 
- You can instead use `xTaskCreateStatic()`, where you need to pass in a reference to the stack.

## Memory Protected Task Creation

- you can optionally create tasks in a memory protected environment, guaranteeing that only the tasks can access it's own data. 
- FreeRTOS offers utilities to take advantage of the On-Board MPU hardware. 

## FreeRTOS - Task States

- We classifies tasks into a bunch of states.

![alt text](image-1.png)

- running: The task is currently running. It's in context
- Read: The task is ready to run
- Blocked: It's waiting for something, like I/O. It doesn't consume CPU time in this state
    - it will transition to ready if the task receives the I/O, or times out. 
- Suspended: Here, the programmer can choose to put a task in the suspend state, via `vTaskSuspend()`, and wake it up via `vTaskResume()`.


## using interrupts for task management

- Let's say we have a task that gets an ADC reading. This takes time, so it'd be a waste to keep the task in the ready state all the time.
- We can use a ISR, or DMA to offload the work from the CPU.
- The ISR can notify the task that there's work to be done