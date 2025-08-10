# 11. Tips for Creating a Well-Abstracted Architecture

## Abstractions - Goal

- Our goal is to create a code base that's usable for a long time, and offers flexibility.
- We want code that we can change overtime without posing barriers. 
- Good comments explaining the APIs.
- the abstraction should be simplistic

```c
bufferX[i] = adc_avg(0,1); //WTF does this mean?
bufferY[i] = adcY->readAdcValue(); //this makes more sense.
```

## Monolithic Architecture

![alt text](image-2.png)

- Here, `algorithm.c` interacts with the ADC directly.
- This isn't ideal, as the usefulness of `algorithm.c` is limited to the MCU and ADC used.

## Abstracted Architecture

![alt text](image-3.png)

- This is the abstracted Architecture.
    - Here, we provide an abstraction driver over the hardware.

- Here, `algorithm.c` interacts with the drivers

## Copy Paste Modify Trap

- This is when you copy a library to multiple projects

![alt text](image-4.png)

- This is less than ideal, because if you need to change the library in one project, you'd have to do it on all projects.