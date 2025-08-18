# 6. Amplifier/Auxiliary Devices

## Amplifiers - representation

- You register the amplifier with an array of `struct snd_soc_aux_dev` types.
    - the `struct snd_soc_card` type has an attribute `aux_dev`, where you insert the array
    - There's also a field, `num_aux_devs` where you insert the size of the array.

```c
struct snd_soc_aux_dev {
	/*
	 * specify multi-codec either by device name, or by
	 * DT/OF node, but not both.
	 */
	struct snd_soc_dai_link_component dlc;

	/* codec/machine specific init - e.g. add machine controls */
	int (*init)(struct snd_soc_component *component);
};
```

## Input Muxing

- There maybe a muxer on the analog input line.
- There are usually:
    - two inputs:  IN1 and IN2
    - one output: OUT
    