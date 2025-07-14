# 9. Introduction to the Timer / Counter Hardware

## how does timer/counter hardware work?

- We'll have a physical system clock, which will be connected to a counter (with the prescaler between obviously).
    - the counter can then be used as a timer or counter, which will store it's count in a register.

- The TCNT register holds the current count value, which you can write to (you can reset it for example.)


## Timer comparison

- You can compare the counter to a value you give.
    - It'll let you know if a certain value has been reached.

- you'd store the value in the **Output Compare Register (OCRn)**
    - once the value is reached, it'll put the microcontroller in it's **waveform mode**.

- you can also register an interrupt handler for them.

## timer - waveform mode

- from the timer, if the compare value is reached, it'll trigger the waveform generator.
    - there are two configuration options:
        - waveform mod
        - output mode
    
- the three waveform modes include:
    - counter
    - timer
    - PWM

- The timer can also trigger an interrupt, which is good for timer based interrupts.

## AVR Timers

- The Atmega series of AVR have three timers:
    - two 8 bit timers
    - one 16 bit timer.

## how to initialize, and read timer

- To initialize the timer:
    - you nede to select the timer mode
    - set clock prescaler.

- The time is stored in the timer register.


## CPU Speed

- The Atmega 168 chip runs at an internal clockspeed of 8 MHz, but it's has a 1/8th prescaler built from the factory bringing the clockspeed down from 8 MHz to 1 MHz.

- You can disable the prescaler by:
    - programming the **fuse**, explained in docs
    - disable the adc hardwarae, via the `power_adc_disable()` function.

## avr - fuse

- the fuse is a programmable flash which controls the behavior of the chip
