# 12. Analog-to-Digital Conversion II

## ADC - Oversampling

- Oversampling is a technique of taking repeated measurements from the ADC and Combining them.

- With the AVR ADC, you can only use 8 bits. If you want to represent more numbers, you can use oversampling to increase precision.


- The basic idea of oversampling is summing up ADC readings some number of times, then dividing it.

## Rounding with integer division

- On computers, integers are rounded down by default.
- To round to the "nearest" integer, you would add half the denominator to the numerator.