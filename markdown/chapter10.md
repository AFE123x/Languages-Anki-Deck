# 10. Pulse Width Modulation (PWM)

## What is PWM?

Pulse Width Modulation (PWM) is a technique that rapidly toggles a digital output between HIGH and LOW states to simulate analog voltage levels. The average voltage output is proportional to the **duty cycle** - the percentage of time the signal is HIGH during each cycle.

**Key Concepts:**
- **Duty Cycle**: Percentage of time signal is HIGH (0-100%)
- **Frequency**: How many complete cycles per second (Hz)
- **Period**: Time for one complete cycle (1/frequency)

**Example:** 50% duty cycle at 5V = 2.5V average output

## PWM Hardware on ATmega328P

The ATmega328P has three timers that can generate PWM:
- **Timer0** (8-bit): 2 PWM channels (OC0A on PB3/Pin 11, OC0B on PB5/Pin 5)
- **Timer1** (16-bit): 2 PWM channels (OC1A on PB1/Pin 9, OC1B on PB2/Pin 10)
- **Timer2** (8-bit): 2 PWM channels (OC2A on PB3/Pin 11, OC2B on PB5/Pin 5)

## PWM Modes

### Fast PWM Mode
- **Characteristics**: Fixed frequency, variable duty cycle
- **Use cases**: Motor control, LED brightness, audio generation
- **TOP value**: 255 (8-bit) or ICR1/OCR1A (16-bit)

### Phase Correct PWM Mode
- **Characteristics**: Half the frequency of Fast PWM, but symmetric waveform
- **Use cases**: Motor control where direction changes are important
- **Advantage**: Reduces noise and provides smoother output

## Register Configuration

- TCCR0A (Timer/Counter Control Register A)
- TCCR0B (Timer/Counter Control Register B)

## Basic PWM Implementation

```c
#include <avr/io.h>

int main(void) {
    // Set PWM pin as output
    DDRB |= (1 << PB3);          // Set OC0A (PB3) as output
    
    // Configure Fast PWM mode
    TCCR0A |= (1 << WGM01) | (1 << WGM00); // Fast PWM mode
    TCCR0A |= (1 << COM0A1);              // Non-inverting mode
    
    // Set prescaler (affects PWM frequency)
    TCCR0B |= (1 << CS01) | (1 << CS00);  // Prescaler 64
    
    // Set duty cycle (0-255 for 8-bit timer)
    OCR0A = 128;  // 50% duty cycle (128/255)
    
    while (1) {
        // PWM runs automatically in hardware
    }
}
```

## PWM Frequency Calculation

**Formula:** f_PWM = f_CPU / (prescaler × 256) for Fast PWM

**Example with 16MHz CPU:**
- Prescaler 1: 62.5 kHz
- Prescaler 8: 7.8 kHz
- Prescaler 64: 976 Hz
- Prescaler 256: 244 Hz
- Prescaler 1024: 61 Hz

## Dual Channel PWM Example

```c
#include <avr/io.h>

int main(void) {
    // Set both PWM pins as outputs
    DDRB |= (1 << PB3) | (1 << PB5);  // OC0A and OC0B
    
    // Configure Fast PWM mode
    TCCR0A |= (1 << WGM01) | (1 << WGM00);
    
    // Enable both PWM channels (non-inverting)
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
    
    // Set prescaler
    TCCR0B |= (1 << CS01) | (1 << CS00);  // Prescaler 64
    
    // Set different duty cycles
    OCR0A = 64;   // 25% duty cycle
    OCR0B = 192;  // 75% duty cycle
    
    while (1) {
        // Both PWM channels run automatically
    }
}
```

## Software PWM with Timer Interrupts

When you need more PWM channels than hardware provides:

```c
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t pwm_counter = 0;
volatile uint8_t duty_cycle_a = 128;
volatile uint8_t duty_cycle_b = 64;

int main(void) {
    // Set LED pins as outputs
    DDRD |= (1 << PD2) | (1 << PD3);
    
    // Configure Timer0 for overflow interrupt
    TCCR0B |= (1 << CS01) | (1 << CS00);  // Prescaler 64
    TIMSK0 |= (1 << TOIE0);               // Enable overflow interrupt
    
    sei();  // Enable global interrupts
    
    while (1) {
        // Main loop can do other tasks
    }
}

ISR(TIMER0_OVF_vect) {
    pwm_counter++;
    
    // Turn on LEDs at start of cycle
    if (pwm_counter == 0) {
        PORTD |= (1 << PD2) | (1 << PD3);
    }
    
    // Turn off LED A when counter reaches duty cycle
    if (pwm_counter == duty_cycle_a) {
        PORTD &= ~(1 << PD2);
    }
    
    // Turn off LED B when counter reaches duty cycle
    if (pwm_counter == duty_cycle_b) {
        PORTD &= ~(1 << PD3);
    }
}
```

## 16-bit Timer PWM

For higher resolution and lower frequencies:

```c
#include <avr/io.h>

int main(void) {
    // Set PWM pin as output
    DDRB |= (1 << PB1);  // OC1A (PB1)
    
    // Configure 16-bit Fast PWM, TOP = ICR1
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
    
    // Non-inverting mode
    TCCR1A |= (1 << COM1A1);
    
    // Set prescaler
    TCCR1B |= (1 << CS11);  // Prescaler 8
    
    // Set TOP value (determines frequency)
    ICR1 = 19999;  // 50Hz with 16MHz/8 prescaler
    
    // Set duty cycle (servo control example)
    OCR1A = 1500;  // 1.5ms pulse width
    
    while (1) {
        // PWM runs automatically
    }
}
```
