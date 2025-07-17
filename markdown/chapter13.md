# 13. Advanced PWM Tricks

## What is a lowpass filter

- PWM creates an average voltage when the response of the driven system is slow enough.
    - This means that running the audio out directly to a speaker, for example, is fine since the speaker doesn't react anywhere as fast as the PWM frequency.

- In contrast, connect the AVR to an amp may cause issues, since the amp may be fast enoug to react to PWM frequency
    - This will cause extra noise or amplifier instability.


- To resolve this, you'd need a low pass filter.

```
AVR output --------+
                   |
                   \
                   /
                   \
                   |
                   +----- audio out
                   |
                   |
                  ____
                  ____
                   |
                   |
                  GND
```