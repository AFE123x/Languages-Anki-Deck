# 5. Serial I/O

## what is a Serial Protocols

- These are a set of rules for encoding data to voltage pulses, and decoding voltage pulses back into data.

## UART Protocol

- This protocol is best described as an analogy with alice and bob.

- Alice has a switch connected to an LED bob has.
- If Alice wants to send a bit to bob, Alice will turn of the LED, so bob knows he should expect a message from alison.


- There are two pins: Transmitter (Tx) and Receiver (Rx)

- You connect the Tx from machine A to machine B's Rx
- You connect the Tx from machine b to machine A's Tx

- The two devices also agree on the number of bits transmitted per second, which is known as the baud rate.


## UART - USART Initialization Registers (ATmega168)

```c
#include <avr/io.h>
#include "USART.h"
#include <util/setbaud.h>

void initUSART(void) {
    UBRR0H = UBRRH_VALUE;   // Set baud rate (high byte)
    UBRR0L = UBRRL_VALUE;   // Set baud rate (low byte)

#if USE_2X
    UCSR0A |= (1 << U2X0);  // Enable double speed mode (optional)
#else
    UCSR0A &= ~(1 << U2X0); // Disable double speed mode
#endif

    UCSR0B = (1 << TXEN0) | (1 << RXEN0); // Enable TX and RX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data frame
}
```

### USART Registers Overview:

* **UBRR0H / UBRR0L (USART Baud Rate Registers)**
  Set the baud rate for serial communication. Together form a 12-bit value.

* **UCSR0A (Control and Status Register A)**

  * `U2X0`: Double transmission speed.
  * `UDRE0`: Transmit buffer empty flag.
  * `RXC0`: Receive complete flag.

* **UCSR0B (Control and Status Register B)**

  * `RXEN0`: Enables receiver.
  * `TXEN0`: Enables transmitter.
  * `RXCIE0`, `TXCIE0`: Enable RX/TX complete interrupts (not used here).

* **UCSR0C (Control and Status Register C)**

  * `UCSZ01`, `UCSZ00`: Character size = 8 bits.
  * Other bits configure parity, stop bits, and frame format.



## UART – Sending & Receiving Bytes (ATmega168)

```c
void transmitByte(uint8_t data) {
    loop_until_bit_is_set(UCSR0A, UDRE0); // Wait until transmit buffer is empty
    UDR0 = data;                          // Send data
}

uint8_t receiveByte(void) {
    loop_until_bit_is_set(UCSR0A, RXC0); // Wait until a byte is received
    return UDR0;                         // Read received byte
}
```

### How it works:

* **`UCSR0A`**: Status register.

  * `UDRE0`: Set when transmit buffer is ready for new data.
  * `RXC0`: Set when unread data is available in receive buffer.

* **`UDR0`**: USART Data Register.

  * Writing to it sends a byte.
  * Reading from it receives a byte.
