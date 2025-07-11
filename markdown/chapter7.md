# 7. Analog to Digital Conversion I

## How does an ADC work?

![](https://media.discordapp.net/attachments/1205002034240163862/1392883981715771392/image.png?ex=687127cb&is=686fd64b&hm=bf26cfc1b7b4b79d0839b972a00b6109f30678c2a5e2cf193f4fb067b157a0e8&=&format=webp&quality=lossless&width=2368&height=1772)

- We have a 10 bit DAC, which will generate an analog signal.
- this signal will be compared with the actual Analog input voltage.
    - if the DAC signal is less than the Analog input, raise the DAC voltage.
    - If it's the other way, decrease.
- it'll continue doing this until it's identical.

- the ADC will have options for a references voltage it applies to the DAC
    - This lets you tailor the voltage range in which the 10 bits are spread.
- There are also triggering option:
    - you can manually start it from your code
    - trigger via events, like overflow conditions
    - it can continuous sample, and run.

## ADC - clock speed

- The ADC module can't run at the full CPU clock speed.
    - usually, there'll be a clock prescaler, that'll divide the cpu clock down to a usable speed.


## configuring ADC

- to make full use of the ADC, you must set:
    - ADC clock prescaler (disabled by default)
    - a reference voltage that defines scale
    - Analog channel to sample from
    - An ADC trigger source (runs freely by default)
    - interrupt to call when ADC conversion is complete
    - other options


## Voltage dividers

```
    vcc
     |
     /
     \ r1
     /
     \
     |
     |
     +---------- Vout
     |
     /
     \ r2
     /
     \
     |
     |
    gnd
```

- this will divide the voltage, which you can calculate with:
    - Vout = Vin * R2 / (R1 + R2)

## ADC - Reading analog signal

```c
int main(void) {
    // -------- Inits --------- //
    uint8_t ledValue;
    uint16_t adcValue;
    uint8_t i;
    initADC0(); // talk in another flashcard
    LED_DDR = 0xff; 
    while (1) {
        ADCSRA |= (1 << ADSC); // step A
        loop_until_bit_is_clear(ADCSRA, ADSC); //step B
        adcValue = ADC; //step C
        ledValue = (adcValue >> 7);
        LED_PORT = 0;
        for (i = 0; i <= ledValue; i++) {
            LED_PORT |= (1 << i);
        }
        _delay_ms(50);
    }
    return (0);
}
```

- A: You need to trigger the start of the read from the ADC.
    - This is done by setting the ADC Start Conversion bit (ADSC) in the ADC Status Register A (ADCSRA).
- B: We read from the ADCSRA register until the ADSC bit is clear.
- C: We read the 10 bit output from the ADC, using the ADC macro.

## ADC - initializing

```c
static inline void initADC0(void) {
    ADMUX |= (1 << REFS0); //sets voltage to 0-5v range
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //sets clock to 125 kHz
    ADCSRA |= (1 << ADEN); //enables ADC
}
```

- `ADMUX` controls the voltage source.
- `ADCSRA` controls the prescaler, enables and starts conversion
- `ADCSRB` controls the triggering.