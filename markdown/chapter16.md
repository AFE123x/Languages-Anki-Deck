# 16. SPI

## What is SPI

- SPI, short for **Serial Peripheral Interface** 
- It's faster than protocols like UART, since it runs at a higher clock speed.
- It's simpler than uart, since you don't need to coordinate the baud rate.
- It consist of 4 wires:
    - one clock line.
    - two data lines
    - signal the start and end of transaction.

## SPI - PINS

- SCK: Clock signal (helpes with synchronizing with data).
- MOSI: Master out, Slave in
- MISO: Master in, Slave out
- Slave select: selects which slave device is active.

## SPI - clock behavior

- On one edge of the clock, the master and slove set up one bit for output
- On the next edge of the clock, the master and slave read one bit as input.

## SPI - Shift Register

- A shift register is a register that reads one bit at a time, and shifts.

- With SPI, devices communicated bit by bit, via shift registers.

![alt text](image-1.png)

- You can see the symmetry.


