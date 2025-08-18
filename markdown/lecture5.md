# 5. ASoC Component Controls

## `snd_soc_component_driver`: `struct snd_kcontrol_new`

- `struct snd_kcontrol_new` is a struct, letting you export "configuration knows" to userspace.

```c
struct snd_kcontrol_new {
	snd_ctl_elem_iface_t iface;	/* interface identifier */
	unsigned int device;		/* device/client number */
	unsigned int subdevice;		/* subdevice (substream) number */
	const char *name;		/* ASCII name of item */
	unsigned int index;		/* index of item */
	unsigned int access;		/* access rights */
	unsigned int count;		/* count of same elements */
	snd_kcontrol_info_t *info;
	snd_kcontrol_get_t *get;
	snd_kcontrol_put_t *put;
	union {
		snd_kcontrol_tlv_rw_t *c;
		const unsigned int *p;
	} tlv;
	unsigned long private_value;
};
```

- There are helper macros to help you fill this out. 

## ASoC Component callbacks

- you can provide callback functions via the `struct soc_dai_ops`


```c
struct snd_soc_dai_ops {
	/* DAI driver callbacks */
	int (*probe)(struct snd_soc_dai *dai);
	int (*remove)(struct snd_soc_dai *dai);
	/* compress dai */
	int (*compress_new)(struct snd_soc_pcm_runtime *rtd);
	/* Optional Callback used at pcm creation*/
	int (*pcm_new)(struct snd_soc_pcm_runtime *rtd,
		       struct snd_soc_dai *dai);

	/*
	 * DAI clocking configuration, all optional.
	 * Called by soc_card drivers, normally in their hw_params.
	 */
	int (*set_sysclk)(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int dir);
	int (*set_pll)(struct snd_soc_dai *dai, int pll_id, int source,
		unsigned int freq_in, unsigned int freq_out);
	int (*set_clkdiv)(struct snd_soc_dai *dai, int div_id, int div);
	int (*set_bclk_ratio)(struct snd_soc_dai *dai, unsigned int ratio);
    // ...
};
```

- You implement the different functions here, like seting the system clock, ppl scaler, etc.

## ASoC, defining PCM bus format

- You define the format using `snd_soc_dai_set_fmt()`.
    - You can choose I2S, right justified, left justified, DSPA/B.

## set_tdm_slot

- this callback configures the DAI for TDM configurations (Time Division Multiplexing).

## Alsa - Trigger
- The callback is called when the stream status is updated.
	- It allows to listen for events.
	- The callback is called from the Alsa core, in soc_pcm_trigger() using snd_soc_pcm_dai_trigger().
	- With DAPM, those clocks are disabled until a stream is ready to be played.
	- A solution is to reset the device when a stream is played.


## bias level

- The callback is called by DAPM through snd_soc_dapm_set_bias_level() and snd_soc_component_set_bias_level() once the component gets activated.
	- It allows to listen for power events.

