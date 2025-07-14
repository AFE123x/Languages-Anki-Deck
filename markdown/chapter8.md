# 8. Hard Interrupts

## polling - what is it, problems, solution

- polling is when you actively check whether a state has changed.
- One problem is that there's no concept of priority.
    - this proves problematic when you have longer event loops, and you can't actively poll as quickly.

- interrupts excel here.

## What do interrupts do?

- interrupts will interrupt the normal flow of program, then call an **interrupt service routine**

## internally/externally trigger interrupts

- internally trigger interrupts respond to an internal AVR peripheral
    - for example, if USART gets new data, or the timer reaches a certain value

- externally triggered interrupts react to voltage change of any of the AVR pins.

## two flavors of interrupts

- There are:
    - fancy interrupts: INT0 and INT1
    - pin change interrupts

- The INT0/1 interrupts are more versatile regarding triggering possibilities: you can interurpt on rising, falling, or change in voltage.

- The Pin Change Interrupt, PCINT, treats all I/O pins as an interrupt source, but only detects changes.
    - this means you need to interpret the change in codee.
    - these interrupts are grouped in channels, for the B, C and D pins.


## INT0 and INT1

- These pins are mainly meant for higher priority interrupts.

## Implementing handler for INT0 interrupt

```c
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(INT0_vect) { // our interrupt handler, registered
    if (bit_is_set(BUTTON_PIN, BUTTON)) {
        PORTB |= (1 << PB0);
        }
        else {
        PORTB &= ~(1 << PB0);
    }
}
/**
* initializes interrupt
*/
void initInterrupt0(void) {
    EIMSK |= (1 << INT0);  //enable the interrupt
    EICRA |= (1 << ISC00); //define when to trigger interrupt (here, it's logical changes)
    sei(); // enable interrupt flag
}

int main(void) {
    // -------- Inits --------- //
    DDRB = 0xff; 
    BUTTON_PORT |= (1 << BUTTON);  //for button pullup
    initInterrupt0();
    while (1) {
        _delay_ms(200);
        LED_PORT ^= (1 << LED0);
    }
}
```

## Pin Change Interrupts

- You use INT0 and INT1 for special needs.
- For all other interrupts, you need to use a **pin-change interrupt**

```c
ISR(PCINT2_vect){
    // handler logic
}

void initPinChangeInterrupt(){
    PCICR |= (1 << PCIE2); //sets pin change interrupt for PORTD

    PCMSK2 |= (1 << PCINT18) //sets pin for PCINT18, or PD2

    sei(); //enable global interrupt bit.
}
```

## global and volatile variables

- global variables are variables that can be accessed by any function defined in the file.
- volatile tells the compiler that the declared variable can change at any time without warning.
    - AKA, the compiler shouldn't change it.