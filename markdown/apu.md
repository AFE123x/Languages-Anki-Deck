# APU

## What is the APU?

- The NES APU is the Audio Processing Unit in the NES that generates sound for games.
- It's registers are mapped in ranges 4000-4013, 4015 and 4017

## Five channels

- The APU has fiwe channels:
    - 2 pulse wave generator
    - Triangle wave
    - Noise Channel
    - Delta Modulation Channel (DMC) for playing DPCM samples

- The DMC plays samples while other's play a waveform.

- Each channel will consist of:
    - variable-rate timer: you can change the timer's speed (for example, to set a new pitch)
    - the variable-rate timer will clock a waveform generator. Once the timer reaches zero, it tells the waveform generator to move to the next step in the waveform.
    - a length counter, which will play sound when the register is non zero.

## APU Status registers

- There are read/write status registers that allow channels to be enabled and disabled.


## Pulse Channel

- The Pulse wave channel ressembles a digital signal.
- It can have a variable duty.

- The pulse channel contains the following:
    - envelope generator
    - sweep unit
    - timer
    - 8 step sequencer
    - length counter


## What is an envolope

- In a synthesizer, an envelope describes the way a sound's parameter changes over time.
    - most significantly, the volume.
    - The common envelope is ADSR: Attack, Decay, Sustain, Release

- The NES APU has a Envelope Generator that controls the volume.
    - It'll automatically reduce the volume over time, in a linear fashion.
    - Optionally, you ca nadd looping (once volume hits zero, reset back to max and start over.)
    - Optionally, you can generate a constant volume, which lets you control it from the software.

## APU Sweep

- The APU Sweep unit is made to adjust the pulse channel period up/down.

- the sweep continuously calculates the pulse channel by:
    - shift the pulse channel raw timer period to the right by the **shift count** specified, that's the change ammount.
    - make change ammount negative if **negative flag** is set.
    - add the current period with the change ammount

- The substraction depends on the pulse channel:
    - pulse1 uses one's complement
    - pulse2 uses two's complement

## APU Sweep - Muting

- The NES APU's pulse channel sweep unit  can cause the pulse channel to be muted under certain conditions.

- It can get muted in two cases:
    - current timer period is less than 8, which is too short a peried to produce a valid tone
    - if the computed target period is greater than `0x7FF`, which is too large a period.

- The sweep unit will continue computing the target period, even if:
    - sweep unit is disabled
    - sweep divider isn't currently clocking.

- to fully disable the sweep unit, you should turn on the Negate flag, so writing `0x08` to the sweep unit.


