# 3. Task Signaling and Communication Mechanisms

## What are RTOS queues

- A queue is simply a data structure that follows the FIFO structure (First In, First Out).
- In RTOS, we can use queues for inter-task communication.
    - while one task puts stuff on the queue, the other task can read from the queue.

- These are the use for RTOS queues.


## RTOS Semaphores

- The semaphore is a powerful tool to indicate samething happened.
- Examples uses include:
    - ISR can give semaphore to let tasks know data's ready for processing.
    - Synchronize tasks (think producer/consumer)
    - restrict the number of simultaneous users of a restricted source.


## counting semaphores

- A counting semaphore is used to manage a shared resource that has a limitation of the number of simultaneous users.

## Binary Semaphore

- A binary semaphore is just a counting semaphore with a max count of 1. 
- They're mainly used for synchronization.

- one task or ISR can give the semaphore to a task to signal it to do some work. 

## RTOS mutexes

- Mutex is short for mutual exclusion.
    - this ensures that two tasks cannot access the same data simultaneously. 
- They're similar to semaphores, but with new features.

## Priority Inversion

- Priority inversion is when a higher priority task has to wait for a lower priority task. 


- One example of this happening is with using semaphores.

![alt text](image.png)

Here, task C could've finished it's work, but has to let task B run since it has a higher priority. this also means task A has to wait since it wants the lock. As a result, A has to wait for B to complete. 

## Mutex - Countering priority inversion

- The mutex introduces a new feature over the binary semaphore, priority inheritence.
- As mentioned before, priority inversion is the problem if lower priority tasks running over higher priority tasks due to lock usage.
- The mutex lock will temporarily promote the task to a higher priority, so it can unlock faster. 