# 1. Introduction

## What is a microcontroller?

- A microcontroller is essentially a computer on a single chip.

- The chip has a CPU, memory, and dedicated serial hardware which lets it communicate with the outer world.

## Microcontroller hardware: big picture

- Microcontrollers consist of pins.
- These pins will have a name (PB0, PB1, etc.)
- These pins are attached to banks, consisting of 8 pins.

- You access said pins by writing to special I/O registers in the chip.

## Microcontroller Components

- The CPU consists of:
    - CPU: Does a bunch of predefined logical and arithmetic operations.
    - memory: there are three types of memory:
        - flash: compiled program is stored here.
        - RAM: You store temporary variables here.
        - EEPROM: it's slow to write to, but is non-volatile.
    - clock: keey sense of time.
    - input/output

## Microcontroller Components - Clocks

- The clock gives a computer a sense of time.
- The AVR chip has multiple clocks, derived from the common timebase, but is divided down through their own prescaler.

- The internal master clock source runs around 8 MHz.
    - from there, the cpu clock is divided down from the master, and runs at 1 MHz by default.
- There are other peripheral clocks for I/O, ODC converter, and the memory.
    - when you use any of these peripheral subsystems, you need to set the prescaler values.

## Microcontoller Peripherals - Serial Communication

- The AVR has three serial communication peripherals built in:
    - USART: used to comunicate with your desktop, radio modems and GPS units.
    - SPI: good for ultra fast communication over very short distances like memory, ADCs and DACs
    - I2C: like a small network, letting you connect up to 127 different devices with the same two wires.

## Microcontroller Peripherals - Analog to Digital Converter

- There are a bunch of sensors that won't be digitally (your microcontrollers native language).
    - Rather, they communicate with an analog signal.
- To deal with this, you use an Analog to Digital converter to convert the analog signal into a digital signal.


## Microcontroller Peripherals - Interrupts

- Hardware interrupts are reactions to certain events (certain amount of time passes, button is pressed).
- An **Interrupt Service Routine** is a software function that you write that executes whenever an interrupt condition is met.
- The CPU will stop what's happening in the main program, run the handler, then return to the program.

## Microcontroller Peripherals - Timers/counters

- The AVR microprocessor will have built in hardware counters.
- These counters keep track of how many times a pin or internal source has changed it's voltage counter.
- One example, is hooking up the internal counter to a button, so it'll tell you how many times the button has been pushed.
- counters are usually paired up with clocks, which are known as **timers**.
- with a timer, you can measure how long some event takes, or the event's frequency.