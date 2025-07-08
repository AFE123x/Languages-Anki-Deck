# 3. Digital Output

## Hardware Registers

```c
DDRB = 0b00000001; //sets Direction to 1
while(1){
    PORTB = 0b00000001; //sets PORTB to 1
}
```

- PORTB and DDRB are addressing pointing to the AVR's internal registers.
- hardware registers are treating like ram, but will have side effects.

- For example, you're controller whether a pin in the PORTB pins is an input or output based on the value of the DDR register
- DDR is the **data direction register**.

## three hardware registers

- **DDRx**: Data Direction register: control which pins are input or output.
    - by default, it's zero.
    - 0 means input, 1 is output
- **PORTx**: Port x Data Register
    - each bit will set the state of the pin
    - 1 is high, 0 is low.
- **PINx**: Port x Input pins
    - you can read whether a pin input is high or low

## `<avr/io.h>` library

- This library contains the PORTs and DDR macros

## `<util/delay.h>` library

- This library provides us the `_delay_ms()` which delays the program by x milliseconsd.