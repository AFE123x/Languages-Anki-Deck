# 8. Digital Input

## What is a pull up resistor

```
avr --------- button ----------+
                               |
                               |
                              GND
```

- in this diagram, if you push the button, it's guaranteed the AVR is at 0 volts.
- if you release the button, the AVR's voltage is undefined
    - We want a defined value

- you could directly connect the AVR directly to VCC, but it'd cause a short if you press the button (no resistance between VCC and GND).
    - instead, you'd insert a pull up resistor.

```
           vcc
            |
            /
            \ 10k ohm resistor
            /
            \
avr --------+---------------- button -----------+
                                                |
                                                |
                                               gnd
```

- here, we have a high value resistor that connects the AVR to vcc.
    - this will **pull up** the voltage to VCC.

- unlike shorting, if the button is pressed, the AVR chooses the path of less resistence, which is ground.

## Configuring input, registers

- you have 3 resistors you'd want to mess around with:
    - DDRx - Data Direction Register: select which pins are input/output
    - PORTx - Here, it enables the pull up resistor for each pin.
    - PINx - you can read the input of the port