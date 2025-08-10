# 9. Drivers and ISRs

## polled drivers, what is it? pros/cons

- Let's say we have a device that takes time to receive data.
- In our driver, we can make a receive, and print task.


```c
void polledUartReceive( void* NotUsed ){    
    uint8_t nextByte;    //setup UART    
    STM_UartInit(USART2, 9600, NULL, NULL);    
    while(1){
        while(!(USART2->ISR & USART_ISR_RXNE_Msk)); 
        nextByte = USART2->RDR;
        xQueueSend(uart2_BytesReceived, &nextByte, 0);    
    }
}
```

```c
void uartPrintOutTask( void* NotUsed){    
    char nextByte;       
    while(1){
        xQueueReceive(uart2_BytesReceived, &nextByte, portMAX_DELAY);
        SEGGER_SYSVIEW_PrintfHost("%c", nextByte);        
    }
}
```

- here, the receive function waits until it completely receives all it's data. 
    - it won't sleep

**pros**
1. Easy to program
2. task has immediate access to data in queue.

**cons**
1. needs to be a high priority task.
2. High chance of data loss.
3. waste of CPU cycles.


## similarities and differences between tasks and ISRs

**similarities**

- both provide a way of achieving parellel code execution
- Only run when required
- can be written in C/C++.

**differences**
- ISRs are brought into context by hardware, while tasks are brought into context by the RTOS kernel.
- ISRs must be concise, tasks are more forgiving.
- ISRs cannot take input parameters. 
- ISRs may only access a lmited ISR specific subset of the FreeRTOS API.
- ISRs operate completely independently of all RTOS code.
- ISRs has it's own stack, each task has it's own stack. 

## Using FreeRTOS API from Interrupts, STM32 interrupts

- Most of the FreeRTOS Apis have `fromISR()` variants.
    - For example, `xQueueSendFromISR()`.

- `fromISR` variants will not block.
    - For example, if `xQueueSendFromISR()` will return immediately if the queue is full.
- Only interrupts with a logically lower priority are permitted to call FreeRTOS API functions.

- STM32 interrupts have a priority range of 0 to 15.
    - priority numbers of 0 to 4 will not be able to use FreeRTOS functions.

- `NVIC(irqtype, priority number)` will set the priority
- `NVIC_EnableIRQ(irqtype)` will enable the interrupt

## Queue Based Drivers

- Here, instead of creating a task to poll the UART registers, we use interrupts to initiate tranfer. 
    - we can use the queue in the IRQ to perform the transfer. 

## Buffer Based drivers

- In this approach, we assume we know the size of the data we're reading. 
- We read the data in the IRQ handler.
- In this case, we can use a semaphore to wake up the thread to process data.

## DMA Based Drivers

- These are meant when you have to transfer large chunks of data.
    - Here, using a queue or buffer based approach would consume cpu cycles.

- DMA based drivers rely on the DMA hardware to transfer bytes from the peripheral read data register into a buffer.


## Stream buffers, API

- this is similar to the queue based system, but closer to the speed of the raw buffer approach to drivers.
- introduced in newer versions of FreeRTOS.
- Instead of sending 1 byte at a tim, you can send multiple

```c
StreamBufferHandle_t xStreamBufferCreate(size_t xBufferSizeBytes, size_t xTriggerLevelBytes);

size_t xStreamBufferReceive(StreamBufferHandle_t* xStreamBuffer, void* buffer, size_t xBufferLengthBytes, TickType_t xTicksToWait);
size_t xStreamBufferSend(StreamBufferHandle_t* streambuffer, void* buffer, size_t length, time);
```
