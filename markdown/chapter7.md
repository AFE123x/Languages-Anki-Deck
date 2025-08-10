# 7. Protecting Data and Synchronizing Tasks

## FreeRTOS - setting up binary semaphore, using

```c
SemaphoreHandle_t semPtr = NULL; // our semaphore type

int main(void){
    semPtr = xSemaphoreCreateBinary(); //creates the semaphore
    assert_param(semPtr != NULL); //returns NULL if fails

    /* creating tasks */
}

void taska(void* arguments){
    uint8_t count = 1;
    while(1){
        if(count++ == 5){
            count = 0;
            xSemaphoreGive(semPtr); //unlocks semaphore, lets task b take lock. 
        }
        else{
            /* whatever */
        }
    }
}

void taskb(void* arguments){
    while(1){
        if(xSemaphoreTake(semPtr, portMAX_DELAY) == pdPass){
            /* if semaphore acquires lock */
        }
        else{
            /* if semaphore doesn't acquire lock.*/
        }
    }
}
```

- On General Purpose systems, semaphores will wait indefinitely until the semaphore is unlocked.
- In a realtime system, you provide a deadline. If the task doesn't acquire the semaphore, it'll give up, and move on. 


## FreeRTOSCounting - Semaphores: setup and using

- Binary semaphores can only have values between 0 and 1.
- counting semaphores let you have a more vast range. 

```c
SemaphoreHandle_t semPtr = NULL;

semPtr = xSemaphoreCreateCounting(max_count, init_count);
assert_param(semPtr != NULL);

if(xSemaphoreTake(semPtr, MAX_TIMEOUT) == pdPass){
    /* task acquires semaphore, decrement count */
}
else{
    /* task doesn't acquire semaphore, count is 0. */
}

xSemaphoreGive(semPtr); //increments counter
```
- `max_count` is the maximum number of threads that can access a certain section.
- `init_count` is the value the semaphore starts at. 

- A semaphore is great if you have 3 different resources for > 3 tasks. Limits tasks going into critical section


## FreeRTOS - Mutex Locks: setup, use

- Mutex Locks work similar to semaphores, but with differences:
    - Mutex Locks temporarily promote task to higher priority to end earlier.
    - unlocking isn't required, since mutexes are initialized with 1

```c
SemaphoreHandle_t MutexPtr = NULL;
MutexPtr = xSemaphoreCreateMutex(); //creates mutex lock
assert_param(mutexPtr != NULL); // checks if allocation succeeds

if(xSemaphoreTake(MutexPtr, MAX_TIMEOUT) == pdPass){
    /* task acquires semaphore, decrement count */
}
else{
    /* task doesn't acquire semaphore, count is 0. */
}

xSemaphoreGive(MutexPtr); //increments counter

```

- The syntax is the same for locking and unlocking lock. 


## Software timers

- Software timers are meant to be an abstraction over the hardware timers.
- Software timers are a handy FreeRTOS features.

- There are two main ways of using software timers:
    - oneshot timers
    - repeat timers


## Oneshot timers

- This is a timer that fires only once.
- It will call a function at a certain time interval.

```c
TimerHandle_t xTimerCreate (
const char * const pcTimerName, //name of timer
const TickType_t xTimerPeriod, //period of timer (how much time)
const UBaseType_t uxAutoReload, //PdFalse for oneshot.
void * const pvTimerID, //A Unique ID For timer, optional
TimerCallbackFunction_t pxCallbackFunction); //function to call upon time elapsed
```

## Repeat Timer

- This is a timer that fires repeatedly
- It will call a function at a certain time interval.

```c
TimerHandle_t xTimerCreate (
const char * const pcTimerName, //name of timer
const TickType_t xTimerPeriod, //period of timer (how much time)
const UBaseType_t uxAutoReload, //PdTrue for repeat timer
void * const pvTimerID, //A Unique ID For timer, optional
TimerCallbackFunction_t pxCallbackFunction); //function to call upon time elapsed
```

- this will call the function repeatedly. 