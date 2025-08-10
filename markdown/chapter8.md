# 8. Intertask Communication

## FreeRTOS - What are queues?

- Queues allow for tasks to communicate between eachother.

## FreeRTOS Queue - initialization and usage

```c
static QueueHandle_t myQueue = NULL; //the queue type

QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize); //creates queue

myQueue = xQueueCreate(2, sizeof(uint8_t)); //creates queue of size two.
assert_param(myQueue != NULL);

uint8_t nextCmd;

//dequeues one item from myQueue, and stores it in nextCmd
if(xQueueReceive(myQueue, &nextCmd, portMAX_DELAY) == pdTrue){
    /* item in queue is stored in nextCmd */
}

uint8_t ledCmd = 0x45;
xQueueSend(myQueue, &ledCmd, portMAX_DELAY); //will enqueue item if it isn't full. If it is, wait until space is available. 
```

## Direct Task Notifications

- Direct tasks are a more lightweight alternative to queues
    - they don't require the communication object to be instantiated in ram.
    - Faster than semaphores and queues

- we can break the direct task notification into two parts:
    - notification itself (works like a semaphore or queue) when unblocking task.
    - 32 bit notification value (optional)

## Direct Task Notification - API

```c
struct xTaskHandle recvTaskHandle = NULL;

retval = xTaskCreate(recvTask, "recvTask",STACK_SIZE, NULL, priority, &recvTaskHandle);
assert_param(retval == pdPASS);
assert_param(recvTaskHandle != NULL);

uint32_t notificationvalue = ulTaskNotifyTake(pdTrue, portMAX_DELAY); //you can then work on the data

xTaskNotify(recvTaskHandle, RED_LED_MASK, eSetValueWithOverwrite); //write notification value with esetValuewithOverwrite
```