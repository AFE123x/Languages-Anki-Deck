# 12. Creating Loose Coupling with Queues

## using queue as interface, why they're good

- We can interface between tasks and peripherals with a queue type. 

- Queues allow for one task to send data, and the other one to receive data.
- There's an abstraction here, as the programmer doesn't have to think abou

- It's a good entry point.


## queue based architecture

- You usually break the architecture into three pieces
    - execution task: reads the queue and executes task.
    - frame protocol decoding: decodes task, and passes it to queue.
    - entry point, where the tasks enter. and sends the data to the frame protocol decoding function.