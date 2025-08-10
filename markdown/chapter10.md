# 10. Sharing Hardware Peripherals Across Tasks

## Shared Peripherals

- Similar to other shared resources, shared peripherals are single items that can be accessed by multiple tasks.
    - Some devices won't support this, so you need to include a mechanism to guarantee orderly access to the resource across tasks. 
- When writing drivers for peripherals, you need to guarantee safe concurrent access
- When timing is critical, the best practice is to use dedicated hardware rather than sharing a peripheral.
    - The stm32 has multiple I2C busses for example.
- When you have high bandwidth peripherals, you'd want to have a peripheral just for it, and you'd ideally use DMA. 

## Goal for peripheral drivers

- We want to make sure that:
    - we can handle tasks trying to access a peripheral concurrently.
    - Data can write to peripherals as quickly as possible.

## Handling device being used - USB example

- In the `CDC_Transmit_FS`, which sends data via the USB protocol, it checks if the USB peripheral is currently in use. 
    - If it is, the transmit fails, and returns.
- How can we efficiently handle it?

### Approach one - polling 
```c
int count = 10;
while(count > 0){
    count--;
    if(CDC_Transmit_FS((uint8_t*)"test\n",5) == USBD_OK){
        break;
    }
    else{
        vTaskDelay(2); //if it fails, wait two seconds and try again.
    }
}
```
- This approach is slow due to the delay.
- If we remove the delay, it's gonna be wasteful to the CPU.

### Approach Two - Stream buffer

- We could use the FreeRTOS Stream buffer to move data to the USB stack.
    - With this approach, we can protect the access to the stream buffer with a mutex.

- This is nice, but requires more ram

### Approach Three - Use FreeRTOS Queue instead of Stream buffer

- Here, we wouldn't need a mutex, it's similar to approach two.
